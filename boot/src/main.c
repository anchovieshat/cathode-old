#include "efi.h"
#include "elf.h"
#include "io.h"

#define KERNEL_MAX_LOAD_ADDR 0x1000000
#define SZOF_PTE 8
#define PAGESHIFT 12
#define PAGESZ (1l << PAGESHIFT)

#define VADDR_L1(l1) ((l1) * PAGESZ)
#define VADDR_L2(l2, l1) (VADDR_L1(l2) * PAGESZ + VADDR_L1(l1))
#define VADDR_L3(l3, l2, l1) (VADDR_L2(l3, 0) * PAGESZ + VADDR_L2(l2, l1))
#define VADDR_L4(l4, l3, l2, l1) (VADDR_L3(l4, 0, 0) * PAGESZ + VADDR_L3(l3, l2, l1))

#define PGNUM_L1(va) (((va) >> PAGESHIFT) & (PAGESZ - 1))
#define PGNUM_L2(va) (((va) >> (2 * PAGESHIFT)) & (PAGESZ - 1))
#define PGNUM_L3(va) (((va) >> (3 * PAGESHIFT)) & (PAGESZ - 1))
#define PGNUM_L4(va) (((va) >> (4 * PAGESHIFT)) & (PAGESZ - 1))

/* PRESENT | WRITABLE | GLOBAL  (! USER) */
#define PF_DEFAULT 259

EfiSystemTable *ST;

static EfiGuid LoadedImageGUID = EFI_LOADED_IMAGE_PROTOCOL_GUID;
static EfiGuid SimpleFileSystemGUID = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
static EfiGuid FileInfoGUID = EFI_FILE_INFO_ID;

extern u64 wstrlen(const u16 *);
extern void *memcpy(void *, const void *, u64);

typedef struct {
	u64 kernel_base;
	EfiMemoryDescriptor *memory_map;
	u64 map_size;
	u64 map_entry_size;
	u64 phy_p4_base;
	u64 phy_pt_pages;
} BootProtocol;

void start(EfiHandle image_handle, EfiSystemTable *sys_table) {
	EfiLoadedImageProtocol *lip;
	EfiSimpleFileSystemProtocol *sfs;
	EfiFileProtocol *root, *kernel_file;
	EfiStatus status;
	u16 *path = u"\\kernel.elf";
	EfiFileInfo *kernel_info;
	usize kernel_info_size = 0;
	u64 kernel_addr = KERNEL_MAX_LOAD_ADDR;
	char *kernel;
	u64 i, j, idx;
	u64 ksize = 0;
	u64 kload = 0xffffffffffffffff;
	u64 kernel_load = KERNEL_MAX_LOAD_ADDR;
	u64 kernel_stack;
	u64 *relptr;
	Elf64_Phdr *kernel_phdr;
	Elf64_Shdr *kernel_shdr;
	Elf64_Shdr *kernel_strshdr;
	Elf64_Ehdr *kernel_hdr;
	Elf64_Rela *rel;
	EfiMemoryDescriptor *mmap;
	usize mmap_size;
	usize mmap_ent_size;
	u32 mmap_ent_ver;
	usize map_key;

	ST = sys_table;

	sys_table->con_out->clear_screen(sys_table->con_out);
	status = sys_table->boot_services->handle_protocol(image_handle, &LoadedImageGUID, (void**)&lip);
	status = sys_table->boot_services->handle_protocol(lip->device_handle, &SimpleFileSystemGUID, (void**)&sfs);

	printf("Loaded at 0x%lux\n", (u64)lip->image_base);

	printf("EFI Version %d.%02d\n", ST->hdr.revision >> 16, ST->hdr.revision & 0xffff);
	printf("EFI Firmware: ");

	sys_table->con_out->output_string(sys_table->con_out, sys_table->fw_vendor);
	printf(" (rev %d.%02d)\n", sys_table->fw_revision >> 16, sys_table->fw_revision & 0xffff);

	status = sfs->open_volume(sfs, &root);

	status = root->open(root, &kernel_file, path, EFI_FILE_MODE_READ, 0);

	status = kernel_file->get_info(kernel_file, &FileInfoGUID, &kernel_info_size, NULL);

	sys_table->boot_services->allocate_pool(EfiLoaderData, kernel_info_size, (void**)&kernel_info);

	status = kernel_file->get_info(kernel_file, &FileInfoGUID, &kernel_info_size, kernel_info);

	//status = sys_table->boot_services->allocate_pages(AllocateMaxAddress, EfiLoaderData, ((kernel_info->file_size+4095) & ~4095)/4096, &kernel_addr);
	status = sys_table->boot_services->allocate_pool(EfiLoaderData, kernel_info->file_size, (void**)&kernel_addr);

	status = kernel_file->read(kernel_file, &kernel_info->file_size, (void*)kernel_addr);
	status = kernel_file->close(kernel_file);

	kernel = (char*)kernel_addr;

	if (kernel[0] == 0x7f && kernel[1] == 'E' && kernel[2] == 'L' && kernel[3] == 'F') {
		kernel_hdr = (Elf64_Ehdr*)kernel;

		kernel_strshdr = (Elf64_Shdr*)(kernel+kernel_hdr->e_shoff+(kernel_hdr->e_shentsize * kernel_hdr->e_shstrndx));

		for (i = 1; i < kernel_hdr->e_shnum; i++) {
			kernel_shdr = (Elf64_Shdr*)(kernel+kernel_hdr->e_shoff+(i*kernel_hdr->e_shentsize));
			printf("Section with name '%s' loads from 0x%lux to 0x%lux\n", kernel+kernel_strshdr->sh_offset+kernel_shdr->sh_name, kernel_shdr->sh_offset, kernel_shdr->sh_addr);
		}
		for (i = 1; i < kernel_hdr->e_phnum; ++i) {
			kernel_phdr = (Elf64_Phdr*)(kernel+kernel_hdr->e_phoff+(i*kernel_hdr->e_phentsize));
			printf("PHdr with type %d\n", kernel_phdr->p_type);
			if (kernel_phdr->p_type == PT_LOAD) {
				printf("Load %lux from 0x%lux to 0x%lux\n", kernel_phdr->p_memsz, kernel_phdr->p_offset, kernel_phdr->p_vaddr);
				if (kernel_phdr->p_vaddr + kernel_phdr->p_memsz > ksize)
					ksize = kernel_phdr->p_vaddr + kernel_phdr->p_memsz;
				if(kernel_phdr->p_vaddr < kload)
					kload = kernel_phdr->p_vaddr;
			}
		}

		status = sys_table->boot_services->allocate_pages(AllocateMaxAddress, EfiLoaderData, (((kernel_info->file_size+(PAGESZ-1)) & ~(PAGESZ-1))/PAGESZ)+64, &kernel_load);
		if (status != EFI_SUCCESS)
			goto fail;

		printf("Kernel virtual extents: %lux low, %lux high\n", kload, ksize);
		printf("Allocated kernel at 0x%lux (to 0x%lux): %d\n", kernel_load, kernel_load+(((kernel_info->file_size+(PAGESZ-1)) & ~(PAGESZ-1))), (i32)status);

		usize imload = (usize) lip->image_base, imhigh = imload + ((usize) lip->image_size);

		usize pages = ((kernel_info->file_size + (PAGESZ-1)) + (lip->image_size + (PAGESZ-1)))/PAGESZ;
		usize pages_pl1 = (pages*SZOF_PTE + PAGESZ - 1)/PAGESZ;
		usize pages_pl2 = (pages_pl1*SZOF_PTE + PAGESZ - 1)/PAGESZ;
		usize pages_pl3 = (pages_pl2*SZOF_PTE + PAGESZ - 1)/PAGESZ;
		usize pages_pl4 = (pages_pl3*SZOF_PTE + PAGESZ - 1)/PAGESZ;
		usize pages_pts = pages_pl1 + pages_pl2 + pages_pl3 + pages_pl4;
		usize *pt_pl4;
		usize *pt_pl3;
		usize *pt_pl2;
		usize *pt_pl1;

		printf("PT: Pages needed: %lud (L1=%lud, L2=%lud, L3=%lud, L4=%lud, %lud to cover kernel)\n", pages_pts, pages_pl1, pages_pl2, pages_pl3, pages_pl4, pages);

		status = sys_table->boot_services->allocate_pages(AllocateAnyPages, EfiLoaderData, pages_pts, &pt_pl4);
		pt_pl3 = (usize *)(((char *) pt_pl4) + SZOF_PTE*pages_pl4);
		pt_pl2 = (usize *)(((char *) pt_pl3) + SZOF_PTE*pages_pl3);
		pt_pl1 = (usize *)(((char *) pt_pl2) + SZOF_PTE*pages_pl2);

		printf("PT: Table pointers at L1=%lux, L2=%lux, L3=%lux, L4=%lux\n", (u64)pt_pl4, (u64)pt_pl3, (u64)pt_pl2, (u64)pt_pl1);
		printf("PT: Granularities at L1=%lux, L2=%lux, L3=%lux, L4=%lux\n", VADDR_L4(0, 0, 0, 1), VADDR_L4(0, 0, 1, 0), VADDR_L4(0, 1, 0, 0), VADDR_L4(1, 0, 0, 0));

		usize vaddr_l, vaddr_h;
		for(u64 i4 = 0; i4 < PAGESZ; i4++) {
			vaddr_l = VADDR_L4(i4, 0, 0, 0);
			vaddr_h = VADDR_L4(i4+1, 0, 0, 0)-1;
			printf("(%lu0x - %lu0x) ", vaddr_l, vaddr_h);
			if((vaddr_h < kload || vaddr_l >= ksize) && (vaddr_h < imload || vaddr_l >= imhigh)) {
				printf(" skipped.\n");
				pt_pl4[i4] = 0;
				continue;
			}
			printf("\n");

			pt_pl4[i4] = (((usize) pt_pl3) & ~(PAGESZ - 1)) | PF_DEFAULT;

			for(u64 i3 = 0; i3 < PAGESZ; i3++) {
				vaddr_l = VADDR_L4(i4, i3, 0, 0);
				vaddr_h = VADDR_L4(i4, i3+1, 0, 0)-1;
				printf("\t(%lu0x - %lu0x) ", vaddr_l, vaddr_h);
				if((vaddr_h < kload || vaddr_l >= ksize) && (vaddr_h < imload || vaddr_l >= imhigh)) {
					printf("skipped.\n");
					pt_pl3[i3] = 0;
					continue;
				}
				printf("\n");

				pt_pl3[i3] = (((usize) pt_pl2) & ~(PAGESZ - 1)) | PF_DEFAULT;

				for(u64 i2 = 0; i2 < PAGESZ; i2++) {
					vaddr_l = VADDR_L4(i4, i3, i2, 0);
					vaddr_h = VADDR_L4(i4, i3, i2+1, 0)-1;
					printf("\t\t(%lu0x - %lu0x) ", vaddr_l, vaddr_h);
					if((vaddr_h < kload || vaddr_l >= ksize) && (vaddr_h < imload || vaddr_l >= imhigh)) {
						printf("skipped.\n");
						pt_pl2[i2] = 0;
						continue;
					}
					printf("\n");

					pt_pl2[i2] = (((usize) pt_pl1) & ~(PAGESZ - 1)) | PF_DEFAULT;

					for(u64 i1 = 0; i1 < PAGESZ; i1++) {
						vaddr_l = VADDR_L4(i4, i3, i2, i1);
						vaddr_h = VADDR_L4(i4, i3, i2, i1+1)-1;
						printf("\t\t\t(%lu0x - %lu0x) ", vaddr_l, vaddr_h);
						if((vaddr_h < kload || vaddr_l >= ksize) && (vaddr_h < imload || vaddr_l >= imhigh)) {
							printf("skipped.\n");
							pt_pl1[i1] = 0;
							continue;
						}

						if(vaddr_h >= kload && vaddr_l < ksize) {
							pt_pl1[i1] = ((vaddr_l - kload + kernel_load) & ~(PAGESZ - 1)) | PF_DEFAULT;
							printf("(region kern) ");
						}
						if(vaddr_h >= imload && vaddr_l < imhigh) {
							pt_pl1[i1] = (vaddr_l & ~(PAGESZ - 1)) | PF_DEFAULT;
							printf("(region boot) ");
						}
						printf("Mapped virt %lu0x -> %lu0x phy\n", vaddr_l, (u64)((pt_pl1[i1] & ~(PAGESZ - 1)) << PAGESHIFT));
					}
				}
			}
		}

		for (i = 1; i < kernel_hdr->e_phnum; ++i) {
			kernel_phdr = (Elf64_Phdr*)(kernel+kernel_hdr->e_phoff+(i*kernel_hdr->e_phentsize));
			if (kernel_phdr->p_type == PT_LOAD) {
				memcpy((void*)kernel_load+kernel_phdr->p_vaddr, kernel+kernel_phdr->p_offset, kernel_phdr->p_filesz);
			}
		}

		status = sys_table->boot_services->allocate_pages(AllocateAnyPages, EfiLoaderData, 512, &kernel_stack);
		printf("Allocated kernel stack at 0x%lux", (u64)kernel_stack);
		if (status != EFI_SUCCESS) {
			goto fail;
		}

		mmap_size = 0;
		sys_table->con_out->clear_screen(sys_table->con_out);

		printf("Booting the kernel...\n");

		sys_table->boot_services->get_memory_map(&mmap_size, NULL, &map_key, &mmap_ent_size, &mmap_ent_ver);
		sys_table->boot_services->allocate_pool(EfiLoaderData, mmap_size, (void**)&mmap);
		sys_table->boot_services->get_memory_map(&mmap_size, mmap, &map_key, &mmap_ent_size, &mmap_ent_ver);
		BootProtocol bootproto = {
			.kernel_base = (u64)kernel_load,
			.memory_map = mmap,
			.map_size = mmap_size,
			.map_entry_size = mmap_ent_size,
			.phy_p4_base = (u64) pt_pl4,
			.phy_pt_pages = pages_pts
		};

		__asm__ volatile ("movq %0, %%cr3" : : "r"(pt_pl4) : );

		if ((status = sys_table->boot_services->exit_boot_services(image_handle, map_key)) != EFI_SUCCESS)
			printf("Failed to exit %d\n", (u32)status);
		else {
			((void (*)(BootProtocol*,void*))(kernel_hdr->e_entry))(&bootproto, (void*)(kernel_stack+(512*4096)));
		}
	}
fail:

	printf("Failed to boot\n");

	printf("\n\nPress any key to poweroff\n");

	sys_table->boot_services->wait_for_event(1, &sys_table->con_in->wait_for_key, &idx);
	printf("System shutdown...\n");
	sys_table->runtime_services->reset_system(EfiResetShutdown, EFI_SUCCESS, 0, NULL);

	for(;;);
}

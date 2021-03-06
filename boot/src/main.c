#include "efi.h"
#include "elf.h"
#include "io.h"

#define KERNEL_MAX_LOAD_ADDR 0x400000000

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

	printf("Loaded at 0x%lx\n", (u64)lip->image_base);

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
			printf("Section with name '%s' loads from 0x%lx to 0x%lx\n", kernel+kernel_strshdr->sh_offset+kernel_shdr->sh_name, kernel_shdr->sh_offset, kernel_shdr->sh_addr);
		}
		for (i = 1; i < kernel_hdr->e_phnum; ++i) {
			kernel_phdr = (Elf64_Phdr*)(kernel+kernel_hdr->e_phoff+(i*kernel_hdr->e_phentsize));
			printf("PHdr with type %d\n", kernel_phdr->p_type);
			if (kernel_phdr->p_type == PT_LOAD) {
				printf("Load %lx from 0x%lx to 0x%lx\n", kernel_phdr->p_memsz, kernel_phdr->p_offset, kernel_phdr->p_vaddr);
				if (kernel_phdr->p_vaddr + kernel_phdr->p_memsz > ksize)
					ksize = kernel_phdr->p_vaddr + kernel_phdr->p_memsz;
			}
		}

        status = sys_table->boot_services->allocate_pages(AllocateMaxAddress, EfiLoaderData, (((kernel_info->file_size+4095) & ~4095)/4096)+64, &kernel_load);
		if (status != EFI_SUCCESS)
			goto fail;

		printf("Allocated kernel at 0x%lx (to 0x%lx): %d\n", kernel_load, kernel_load+(((kernel_info->file_size+4095) & ~4095)), (i32)status);

		for (i = 1; i < kernel_hdr->e_phnum; ++i) {
			kernel_phdr = (Elf64_Phdr*)(kernel+kernel_hdr->e_phoff+(i*kernel_hdr->e_phentsize));
			if (kernel_phdr->p_type == PT_LOAD) {
				memcpy((void*)kernel_load+kernel_phdr->p_vaddr, kernel+kernel_phdr->p_offset, kernel_phdr->p_filesz);
			}
		}
		for (i = 1; i < kernel_hdr->e_shnum; i++) {
			kernel_shdr = (Elf64_Shdr*)(kernel+kernel_hdr->e_shoff+(i*kernel_hdr->e_shentsize));
			if (kernel_shdr->sh_type == SHT_RELA) {
				printf("%ld relocations\n", kernel_shdr->sh_size / kernel_shdr->sh_entsize);
				for (j = 0; j < kernel_shdr->sh_size / kernel_shdr->sh_entsize; ++j) {
					rel = (Elf64_Rela*)(kernel+kernel_shdr->sh_offset+(j*kernel_shdr->sh_entsize));
					if (ELF64_R_TYPE(rel->r_info) == R_X86_64_RELATIVE) {
						if ((u64)relptr > (u64)((char*)kernel_load)+(4096*((kernel_info->file_size+4095) & ~4095))) {
								printf("Reloc past kernel end\n");
						}
						relptr = (u64*)(kernel_load+rel->r_offset);
						*relptr = (u64)(kernel_load+rel->r_addend);
					} else
						printf("Relocation type 0x%x at 0x%lx\n", ELF64_R_TYPE(rel->r_info), rel->r_offset);
				}
			}
		}

		status = sys_table->boot_services->allocate_pages(AllocateAnyPages, EfiLoaderData, 512, &kernel_stack);
		printf("Allocated kernel stack at 0x%lx", (u64)kernel_stack);
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
			.map_entry_size = mmap_ent_size
		};

		if ((status = sys_table->boot_services->exit_boot_services(image_handle, map_key)) != EFI_SUCCESS)
			printf("Failed to exit %d\n", (u32)status);
		else {
			((void (*)(BootProtocol*,void*))(kernel_load))(&bootproto, (void*)(kernel_stack+(512*4096)));
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

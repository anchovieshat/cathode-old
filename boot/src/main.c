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

void start(EfiHandle image_handle, EfiSystemTable *sys_table) {
	EfiLoadedImageProtocol *lip;
	EfiSimpleFileSystemProtocol *sfs;
	EfiFileProtocol *root, *kernel_file;
	EfiStatus status;
	usize idx;
	u16 *path = u"\\kernel.elf";
    EfiFileInfo *kernel_info;
	usize kernel_info_size = 0;
	u64 kernel_addr = KERNEL_MAX_LOAD_ADDR;
	char *kernel;
	u32 i;
	Elf64_Phdr *kernel_phdr;

	Elf64_Ehdr *kernel_hdr;

	ST = sys_table;

	sys_table->con_out->clear_screen(sys_table->con_out);
	status = sys_table->boot_services->handle_protocol(image_handle, &LoadedImageGUID, (void**)&lip);
	status = sys_table->boot_services->handle_protocol(lip->device_handle, &SimpleFileSystemGUID, (void**)&sfs);

	status = sfs->open_volume(sfs, &root);

	status = root->open(root, &kernel_file, path, EFI_FILE_MODE_READ, 0);
	printf("Open kernel: %d\n", (i32)status);

	status = kernel_file->get_info(kernel_file, &FileInfoGUID, &kernel_info_size, NULL);
	printf("Needed size %d\n", (i32)kernel_info_size);

    sys_table->boot_services->allocate_pool(EfiLoaderData, kernel_info_size, (void**)&kernel_info);

	status = kernel_file->get_info(kernel_file, &FileInfoGUID, &kernel_info_size, kernel_info);
	printf("Kernel get info: %d\n", (i32)status);
	printf("Kernel file size: %d\n", (i32)kernel_info->file_size);

	status = sys_table->boot_services->allocate_pages(AllocateMaxAddress, EfiLoaderData, ((kernel_info->file_size+4095) & ~4095)/4096, &kernel_addr);
	printf("kernel alloc: %d at 0x%lx\n", (i32)status, kernel_addr);

	status = kernel_file->read(kernel_file, &kernel_info->file_size, (void*)kernel_addr);
	printf("kernel read: %d\n", (i32)status);
	status = kernel_file->close(kernel_file);
	printf("kernel closed: %d\n", (i32)status);

    kernel = (char*)kernel_addr;

	if (kernel[0] == 0x7f && kernel[1] == 'E' && kernel[2] == 'L' && kernel[3] == 'F') {
		printf("Looks like a valid kernel to me, y'know\n");
		kernel_hdr = (Elf64_Ehdr*)kernel;
		printf("Our entry is 0x%lx\n", kernel_hdr->e_entry);
		printf("Our type is %d\n", (u32)kernel_hdr->e_type);
		printf("There are %d PSections\n", kernel_hdr->e_phnum);
		for (i = 0; i < kernel_hdr->e_phnum; ++i) {
			kernel_phdr = (Elf64_Phdr*)(kernel + kernel_hdr->e_phoff + (kernel_hdr->e_phentsize * i));
			printf("PSection with type 0x%x\n", kernel_phdr->p_type);
		}
	}

	printf("Loaded at 0x%lx\n", (u64)lip->image_base);

	printf("EFI Version %d.%02d\n", ST->hdr.revision >> 16, ST->hdr.revision & 0xffff);
	printf("EFI Firmware: ");

	sys_table->con_out->output_string(sys_table->con_out, sys_table->fw_vendor);
	printf(" (rev %d.%02d)\n", sys_table->fw_revision >> 16, sys_table->fw_revision & 0xffff);


	printf("\n\nPress any key to poweroff\n");

	sys_table->boot_services->wait_for_event(1, &sys_table->con_in->wait_for_key, &idx);
	printf("System shutdown...\n");
	sys_table->runtime_services->reset_system(EfiResetShutdown, EFI_SUCCESS, 0, NULL);

	for(;;);
}

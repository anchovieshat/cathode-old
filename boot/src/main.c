#include "efi.h"
#include "io.h"

EfiSystemTable *ST;

void start(EfiHandle image_handle, EfiSystemTable *sys_table) {
	usize idx;
	ST = sys_table;

	sys_table->con_out->clear_screen(sys_table->con_out);
	printf("Loaded at 0x%lx\n", (u64)image_handle);
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

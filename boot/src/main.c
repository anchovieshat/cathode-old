#include "efi.h"
#include "io.h"

EfiSystemTable *ST;

static EfiGuid LoadedImageGUID = EFI_LOADED_IMAGE_PROTOCOL_GUID;
static EfiGuid LoadedImageDevicePathGUID = EFI_LOADED_IMAGE_DEVICE_PATH_PROTOCOL_GUID;
static EfiGuid DevicePathToTextGUID = EFI_DEVICE_PATH_TO_TEXT_PROTOCOL_GUID;

void start(EfiHandle image_handle, EfiSystemTable *sys_table) {
	usize idx;
	EfiDevicePathToTextProtocol *dpt;
	EfiDevicePathProtocol *dpp;
	EfiLoadedImageProtocol *lip;
	EfiStatus status;
	u16 *path;
	ST = sys_table;

	sys_table->con_out->clear_screen(sys_table->con_out);
	//status = sys_table->boot_services->open_protocol(image_handle, &LoadedImageGUID, (void**)&lip, image_handle, NULL, 0);
	status = sys_table->boot_services->handle_protocol(image_handle, &LoadedImageGUID, (void**)&lip);
	status = sys_table->boot_services->handle_protocol(image_handle, &LoadedImageDevicePathGUID, (void**)&dpp);
	status = sys_table->boot_services->locate_protocol(&DevicePathToTextGUID, NULL, (void**)&dpt);



	printf("Loaded at 0x%lx\n", (u64)lip->image_base);

	printf("Loaded from: ");
	path = dpt->ConvertDevicePathToText(dpp, 0, 0);
	sys_table->con_out->output_string(sys_table->con_out, path);

	printf("\nEFI Version %d.%02d\n", ST->hdr.revision >> 16, ST->hdr.revision & 0xffff);
	printf("EFI Firmware: ");
	sys_table->con_out->output_string(sys_table->con_out, sys_table->fw_vendor);
	printf(" (rev %d.%02d)\n", sys_table->fw_revision >> 16, sys_table->fw_revision & 0xffff);


	printf("\n\nPress any key to poweroff\n");

	sys_table->boot_services->wait_for_event(1, &sys_table->con_in->wait_for_key, &idx);
	printf("System shutdown...\n");
	sys_table->runtime_services->reset_system(EfiResetShutdown, EFI_SUCCESS, 0, NULL);

	for(;;);
}

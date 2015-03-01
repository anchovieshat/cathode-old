#include "efi.h"

void start(EfiHandle image_handle, EfiSystemTable *sys_table) {
	sys_table->con_out->clear_screen(sys_table->con_out);
	sys_table->con_out->output_string(sys_table->con_out, u"Hello world...");

	for(;;);
}

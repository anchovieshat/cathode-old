SECTIONS {
	. = 0;

	.boot : {
		*(.boot)
	}

	. = ALIGN(4096);

	.text : {
		*(.text);
		*(.text*);
	}

	. = ALIGN(4096);

	.rodata : {
		*(.rodata);
		*(.rodata*);
	}

	. = ALIGN(4096);

	.data : {
		*(.data);
	}

	. = ALIGN(4096);

	.eh_frame : {
		*(.eh_frame);
	}

	. = ALIGN(4096);

	.bss : {
		*(.bss);
		*(COMMON);
	}
}

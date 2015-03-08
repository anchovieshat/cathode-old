ENTRY(_start)
MEMORY {
	kernel : org = 0xffffffffc0000000, len = 0x3fffffff
}
SECTIONS {

	. = ORIGIN(kernel) + SIZEOF_HEADERS;

	. = ALIGN(4096);

	.boot : {
		_start
		*(.boot)
	}

	. = ALIGN(4096);

	.text : {
		*(.text)
		*(.text*)
	}

	. = ALIGN(4096);

	.rodata : {
		*(.rodata*)
	}

	. = ALIGN(4096);

	.data : {
		*(.data)
		*(.data*)
	}

	. = ALIGN(4096);

	.data.rel.ro : {
		*(.data.rel.ro*)
	}

	. = ALIGN(4096);

	.eh_frame : {
		*(.eh_frame)
	}

	. = ALIGN(4096);

	.bss : {
		*(.bss)
		*(COMMON)
	}

	. = ALIGN(4096);

	.rela : {
		*(.rela*)
	}

	/DISCARD/ : {
		*(.comment*)
		*(.note*)
	}
}

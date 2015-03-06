ENTRY(_start)
SECTIONS {
	. = 0;

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

	.eh_frame : {
		*(.eh_frame)
	}

	. = ALIGN(4096);

	.bss : {
		*(.bss)
		*(COMMON)
	}

	/DISCARD/ : {
		*(.comment*)
		*(.note*)
	}
}

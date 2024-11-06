SECTIONS
{
  . = 0x80000000;			/* Загрузочный адрес QEMU для запуска bios */
  .text : {
		KEEP(*(.text._start));  /* Ensure _start is placed first */
		*(.text*);			
	}

    . = ALIGN (CONSTANT (COMMONPAGESIZE));

  .data : { 
		*(.data*)
	}
}
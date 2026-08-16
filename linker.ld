ENTRY(_start)

SECTIONS
{
  /* Place kernel 2MB above the start of memory (OpenSBI is at 0x80000000) */
  . = 0x80200000;

  _kernel_start = .;
  .text : {
    KEEP(*(.text.kentry))
    *(.text .text.*)
  }

  . = ALIGN(4K);

  .rodata : {
    *(.rodata .rodata.*)
  }

  . = ALIGN(4K);

  .data : {
    *(.data .data.*)
    *(.sdata .sdata.*)
  }

  . = ALIGN(4K);

  _bss_start = .;
  .bss (NOLOAD) : {
    *(.bss .bss.*)
    *(COMMON)
  }
  _bss_end = .;

  . = ALIGN(16);

  .stack (NOLOAD) : {
    _stack_bottom = .;
    . = . + 4K;
    _stack_top = .;
  }

  . = ALIGN(4K);

  _kernel_end = .;
}

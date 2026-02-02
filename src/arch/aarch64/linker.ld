ENTRY(_start)

SECTIONS
{
    . = 0x80000;

    .text : {
        KEEP(*(.text.boot))
        *(.text*)
    }

    .rodata : {
        *(.rodata*)
    }

    . = ALIGN(16);
    .data : {
        *(.data*)
    }

    . = ALIGN(16);
    .bss (NOLOAD) : {
        _bss_start = .;
        *(.bss*)
        *(COMMON)
        _bss_end = .;
    }

    . = ALIGN(16);
    .stack (NOLOAD) : {
        _stack_bottom = .;
        *(.stack)
        _stack_top = .;
    }

    . = ALIGN(16);
    .heap (NOLOAD) : {
        _heap_start = .;
        . += 0x100000;
        _heap_end = .;
    }
    . = ALIGN(16);
    _kernel_end = .;
}


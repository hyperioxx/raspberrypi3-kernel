ENTRY(_start)

SECTIONS
{
    . = 0x80000; 

    .text : {
        KEEP(*(.text.boot))
        *(.text)
    }

    .rodata : {
        *(.rodata*)
     }
     . = ALIGN(16);    
    .bss : {
         _bss_start = .;
        *(.bss*)
        *(COMMON)
        _bss_end = .;
    }
    .stack (NOLOAD) : ALIGN(16) {
        *(.stack)
    }
}

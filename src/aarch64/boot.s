.global _start
.section .text.boot
_start:
   LDR X0, =_stack_top
   MOV SP, X0
   MOV X29, XZR
   LDR X1, =_bss_start      
   LDR X2, =_bss_end        

bss_clear:
    CMP X1, X2
    B.HS bss_done           

    STR XZR, [X1], #8
    B bss_clear

bss_done:
    BL kernel_main

.section .stack, "aw", %nobits
.align 12
_stack:
    .space 4096
_stack_top:

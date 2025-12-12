/* 
   for the EL modes/MMU I used the following for some guidance: 
       https://dannasman.github.io/aarch64-mmu.html
       https://krinkinmu.github.io/2021/01/10/aarch64-interrupt-handling.html
   if I ever support other aarch64 cpus then will need to handle EL3  
*/
.global _start
.section .text.boot
_start:
    BL check_el
loop:
    B loop

check_el: 
    MRS x0, CurrentEL
    CMP x0, #0x8 
    BEQ in_el2 // if we start higher (EL3) just return we can come back for EL3 in the future
    RET

in_el2:
    mrs     x0, hcr_el2
    orr     x0, x0, (1 << 31) 
    and     x0, x0, ~(1 << 5)
    and     x0, x0, ~(1 << 4)
    and     x0, x0, ~(1 << 3)
    msr     hcr_el2, x0
    mov     x0, 0b00101
    msr     spsr_el2, x0
    adr     x0, in_el1
    msr     elr_el2, x0
    eret

in_el1: 
    BL bss_setup
    BL bss_clear
    BL setup_c_env
    BL vector_table_setup
    BL kernel_main


setup_c_env:
   LDR X0, =_stack_top
   MOV SP, X0
   MOV X29, XZR
   RET

bss_setup: 
   LDR X1, =_bss_start      
   LDR X2, =_bss_end
   RET

bss_clear:
    CMP X1, X2
    BHS bss_done           
    STR XZR, [X1], #8
    B bss_clear

bss_done:
    RET

vector_table_setup:
    LDR x0, =vector_table
    MSR VBAR_EL1, x0
    ISB
    RET

.align 11
.global vector_table
vector_table:
    b sync_sp0
    .space 0x80 - 4
    b irq_sp0
    .space 0x80 - 4
    b fiq_sp0
    .space 0x80 - 4
    b serr_sp0
    .space 0x80 - 4

    b handle_interrupt
    .space 0x80 - 4
    b handle_interrupt
    .space 0x80 - 4
    b handle_interrupt
    .space 0x80 - 4
    b handle_interrupt
    .space 0x80 - 4

sync_sp0:  b sync_sp0
irq_sp0:   b irq_sp0
fiq_sp0:   b fiq_sp0
serr_sp0:  b serr_sp0


handle_interrupt:
    // save minimal caller-saved registers
    sub sp, sp, #16
    stp x0, x1, [sp]

    bl handle_interrupt_c

    ldp x0, x1, [sp]
    add sp, sp, #16

    eret    
    

.section .stack, "aw", %nobits
.align 12
_stack:
    .space 4096
_stack_top:

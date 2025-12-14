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
    orr     x0, x0, (1 << 31) // enables aarch64 for E1 mode 
    and     x0, x0, ~(1 << 5) // allow EL1 mode to handle aborts 
    and     x0, x0, ~(1 << 4) // allow EL1 mode to handle interrupts
    and     x0, x0, ~(1 << 3) // allow EL1 mode to handle fast interrupts
    msr     hcr_el2, x0
    mrs     x0, cnthctl_el2
    orr     x0, x0, #(1 << 0) // allow EL1 to read timer       
    orr     x0, x0, #(1 << 1) // allow EL1 to change timer       
    msr     cnthctl_el2, x0
    mov     x0, 0b00101
    msr     spsr_el2, x0
    adr     x0, in_el1
    msr     elr_el2, x0
    eret

in_el1: 
    BL setup_c_env
    BL vector_table_setup
    BL bss_setup
    BL bss_clear
    BL kernel_main
hang:
    WFI
    B hang

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
    sub sp, sp, #272

    stp x0,  x1,  [sp, #16*0]
    stp x2,  x3,  [sp, #16*1]
    stp x4,  x5,  [sp, #16*2]
    stp x6,  x7,  [sp, #16*3]
    stp x8,  x9,  [sp, #16*4]
    stp x10, x11, [sp, #16*5]
    stp x12, x13, [sp, #16*6]
    stp x14, x15, [sp, #16*7]
    stp x16, x17, [sp, #16*8]
    stp x18, x19, [sp, #16*9]
    stp x20, x21, [sp, #16*10]
    stp x22, x23, [sp, #16*11]
    stp x24, x25, [sp, #16*12]
    stp x26, x27, [sp, #16*13]
    stp x28, x29, [sp, #16*14]
    stp x30, xzr, [sp, #16*15]   // padding

    mrs x9,  elr_el1
    mrs x10, spsr_el1
    str x9,  [sp, #256]          // ELR_EL1
    str x10, [sp, #264]          // SPSR_EL1

    mov x0, sp                   // x0 = struct trap_frame *
    bl handle_interrupt_c

    ldr x9,  [sp, #256]
    ldr x10, [sp, #264]
    msr elr_el1,  x9
    msr spsr_el1, x10

    ldp x30, xzr, [sp, #16*15]
    ldp x28, x29, [sp, #16*14]
    ldp x26, x27, [sp, #16*13]
    ldp x24, x25, [sp, #16*12]
    ldp x22, x23, [sp, #16*11]
    ldp x20, x21, [sp, #16*10]
    ldp x18, x19, [sp, #16*9]
    ldp x16, x17, [sp, #16*8]
    ldp x14, x15, [sp, #16*7]
    ldp x12, x13, [sp, #16*6]
    ldp x10, x11, [sp, #16*5]
    ldp x8,  x9,  [sp, #16*4]
    ldp x6,  x7,  [sp, #16*3]
    ldp x4,  x5,  [sp, #16*2]
    ldp x2,  x3,  [sp, #16*1]
    ldp x0,  x1,  [sp, #16*0]

    add sp, sp, #272
    eret

.section .stack, "aw", %nobits
.align 12
_stack:
    .space 4096
_stack_top:

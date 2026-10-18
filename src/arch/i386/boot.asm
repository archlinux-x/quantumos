; =============================================================================
; boot.asm - Multiboot header and kernel entry point
; =============================================================================

MBALIGN     equ  1 << 0             ; Align loaded modules on page boundaries
MEMINFO     equ  1 << 1             ; Provide memory map
FLAGS       equ  MBALIGN | MEMINFO  ; Multiboot flags
MAGIC       equ  0x1BADB002         ; Multiboot magic number
CHECKSUM    equ -(MAGIC + FLAGS)    ; Checksum: MAGIC + FLAGS + CHECKSUM = 0

section .multiboot
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

section .bss
align 16
stack_bottom:
    resb 16384                      ; 16 KB stack
stack_top:

section .text
global _start
extern kernel_main

_start:
    ; Initialize stack pointer
    mov esp, stack_top

    ; Push multiboot information pointer (ebx) and magic (eax)
    push ebx
    push eax

    ; Reset EFLAGS
    push 0
    popf

    ; Call main kernel entry point
    call kernel_main

    ; If kernel_main returns, disable interrupts and halt
    cli
.hang:
    hlt
    jmp .hang

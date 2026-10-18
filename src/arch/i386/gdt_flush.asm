; =============================================================================
; gdt_flush.asm - Loads GDT pointer and reloads segment registers
; =============================================================================

global gdt_flush

section .text
gdt_flush:
    mov eax, [esp + 4]  ; Pointer to gdt_ptr
    lgdt [eax]          ; Load GDT

    ; Reload data segments
    mov ax, 0x10        ; Kernel data segment selector (offset 16)
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Far jump to reload CS to 0x08 (Kernel code segment)
    jmp 0x08:.flush
.flush:
    ret

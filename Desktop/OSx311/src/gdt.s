[GLOBAL gdt_flush]

gdt_flush:
    mov eax, [esp+4]
    lgdt [eax]		; Load new GDT

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:.flush
.flush:
    ret

[GLOBAL idt_flush]

idt_flush:
    mov eax, [esp+4]
    lidt [eax]		; Load new IDT
    ret

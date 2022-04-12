global _load_idt_asm

_load_idt_asm:
    lidt    [rdi]   ; load IDT -> rdi is the first argument passed
    sti		    ; store interrupts
    ret		  
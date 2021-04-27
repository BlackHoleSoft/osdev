[org 0x7c00]

mov ah, 0x0e

mov bx, test_str
call printf

jmp $

%include "printf.asm"

test_str:
    db 'Test string', 0

times 510-($-$$) db 0

dw 0xaa55
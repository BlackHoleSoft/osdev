[org 0x7c00]

mov ah, 0x0e

mov bx, str_step_1
call printf16

call load_disk16

mov bx, str_step_2
call printf16

call switch_pm

jmp $

%include "printf.asm"
%include "disk16.asm"

str_step_1:
    db 'Step 0', 0

times 510-($-$$) db 0

dw 0xaa55

str_step_2:
    db ' --> Step 1: Extra code area loaded', 0

%include "pm.asm"

[bits 32]
%include "main.asm"

times 8*1024-($-$$) db 0
;; Prints string from address in bx
printf16:
    pusha    
print_loop:
    mov al, [bx]  
    cmp al, 0
    je print_ret    
    int 0x10
    inc bx
    jmp print_loop
print_ret:
    popa
    ret
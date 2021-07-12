;;load sectors from disk
load_disk16:
    ;mov dl, 0
    pusha
    mov ah, 0x02
    mov al, 0x0f    ;sectors count
    mov ch, 0x00    ;cylinder
    mov cl, 0x02    ;sector
    mov dh, 0x00    ;head
    mov bx, 0x0     ;segment load to
    mov es, bx
    mov bx, 0x7e00  ;offset
    int 0x13        ;call disk read
    jc disk_error
    cmp al, 0x0
    je disk_error_no_sec
    popa
    ret

disk_error:
    mov bx, err_str
    call printf16
    jmp $

disk_error_no_sec:
    mov bx, err_str_no_sec
    call printf16
    jmp $

err_str:
    db 'Disk error!', 0
err_str_no_sec:
    db 'Disk error. Return 0 sectors!', 0
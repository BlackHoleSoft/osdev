global start
extern kmain

section .bss
align 4096
; init stack
stack_bottom:
	resb 4096 * 4
stack_top:

section .text
bits 32
start:
	mov esp, stack_top

	mov eax, 0xb8000

prnt_loop:

	mov dword [eax], 0x20202020
	add eax, 4
	cmp eax, 0xb8020
	jne prnt_loop

; print `OK`
	mov dword [eax], 0x204b204f
	add eax, 4

prnt_loop_2:

	mov dword [eax], 0x20202020
	add eax, 4
	cmp eax, 0xb8044
	jne prnt_loop_2

load_kernel:

	; load null into all data segment registers
    ;mov ax, 0
    ;mov ss, ax
    ;mov ds, ax
    ;mov es, ax
    ;mov fs, ax
    ;mov gs, ax

	call kmain
	
	hlt

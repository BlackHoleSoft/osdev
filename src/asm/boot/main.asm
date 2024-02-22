global start

section .text
bits 32
start:

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
	
	hlt

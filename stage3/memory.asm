global memcpy, memmove

section .text

memcpy:
.bulk_copy:
	cmp rdx, 8
	jl .bytewise_copy
	mov rax, qword[rsi]
	mov qword[rdi], rax
	sub rdx, 8
	add rdi, 8
	add rsi, 8
.bytewise_copy:
	cmp rdx, 0
	je .return
	mov al, byte[rsi]
	mov byte[rdi], al
	dec rdx
	inc rdi
	inc rsi
	jmp .bytewise_copy
.return:
	ret

memmove:
	mov rcx, rdx
.bulk_read:
	cmp rdx, 8
	jl .bytewise_read
	push qword[rsi]
	add rsi, 8
	sub rdx, 8
	jmp .bulk_read
.bytewise_read:
	cmp rdx, 0
	je .bulk_write
	dec rsp
	mov al, byte[rsi]
	mov byte[rsp], al
	inc rsi
	dec rdx
	jmp .bytewise_read
.bulk_write:
	cmp rcx, 8
	jl .bytewise_write
	pop qword[rdi]
	add rdi, 8
	sub rcx, 8
.bytewise_write:
	cmp rcx, 0
	je .return
	mov al, byte[rsp]
	mov byte[rdi], al
	inc rsp
	inc rdi
	dec rcx
.return:
	ret

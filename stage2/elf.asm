load_kernel_elf:
	mov eax, [kernel_elf]
	cmp eax, [.elf]
	jne .fail

	mov r8, [kernel_elf+32] ; program header pos
	add r8, kernel_elf

	movzx rbx, word[kernel_elf+54] ; program header size
	movzx rdx, word[kernel_elf+56] ; num of program headers

.header:
	cmp rdx, 0
	je .done

	mov edi, [r8] ; type
	cmp edi, 0
	je .next

	cmp edi, 1
	je .load

	cmp edi, 2
	je .fail

	cmp edi, 3
	je .fail

	jmp .next

.load:
	; zero out the segment
	mov al, 0
	mov rdi, [r8+16]
	mov rcx, [r8+40]
	rep stosb

	; load from file
	mov rdi, [r8+16]
	mov rsi, [r8+8]
	add rsi, kernel_elf
	mov rcx, [r8+32]
	rep movsb

.next:

	add r8, rbx
	dec rdx
	jmp .header

.done:

	; ELF entry point
	mov rax, [kernel_elf+24]
	ret

.fail:
	cli
	hlt
	jmp $

.elf: db 0x7f, "ELF"

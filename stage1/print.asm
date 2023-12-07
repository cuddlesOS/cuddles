; uses eax, ebx
print_str:
	mov ah, 0x0E
.print:
	mov al, [ebx]
	cmp al, 0
	je .return
	int 0x10
	inc ebx
	jmp .print
.return:
	ret

; modify eax, ebx, ecx, edx
print_hex:
	mov ebx, 0x10
	jmp print_num
print_dec:
	mov ebx, 10
print_num:
	xor ecx, ecx
.convert:
	inc ecx
	xor edx, edx
	div ebx
	cmp dl, 10
	jb .digit
	add dl, 'A'-10
	jmp .next
.digit:
	add dl, '0'
.next:
	push dx
	cmp eax, 0
	jne .convert
.print:
	cmp ecx, 0
	je .return
	dec ecx
	pop ax
	mov ah, 0x0E
	int 0x10
	jmp .print
.return:
	ret

newline:
	mov al, 10
	call print_chr

	mov al, 13
	call print_chr

	ret

print_chr:
	mov ah, 0x0E
	int 0x10
	ret

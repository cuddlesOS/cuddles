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

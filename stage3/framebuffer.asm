%include "stage3/colors.asm"
global print_chr, print_str, print_dec, print_hex, clear_screen, newline
extern memcpy

section .data

pos:
.row: db 0
.col: db 0

cursor: dq 0xB8000
color: db COLOR_WHITE | (COLOR_BLACK << 4)

section .text

set_color:
	shl sil, 4
	add dil, sil
	mov [color], dil

update_cursor:
	mov rbx, [cursor]
	sub rbx, 0xB8000
	shr rbx, 1

	mov dx, 0x3D4
	mov al, 14
	out dx, al

	mov dx, 0x3D5
	mov al, bh
	out dx, al

	mov dx, 0x3D4
	mov al, 15
	out dx, al

	mov dx, 0x3D5
	mov al, bl
	out dx, al

	ret

set_chr:
	mov rax, [cursor]
	mov byte[rax], dil
	inc rax
	mov dil, [color]
	mov [rax], dil
	inc rax
	mov [cursor], rax
	jmp update_cursor

vertical_tab:
	mov al, [pos.row]
	inc al
	cmp al, 25
	je .scroll
	mov [pos.row], al
	mov rax, [cursor]
	add rax, 160
	mov [cursor], rax
	jmp update_cursor
.scroll:
	mov rdi, 0xB8000
	mov rsi, 0xB80A0
	mov rdx, 0xF00
	jmp memcpy

carriage_return:
	mov rax, [cursor]
	xor rbx, rbx
	mov bl, [pos.col]
	shl bl, 1
	sub rax, rbx
	mov [cursor], rax
	mov byte[pos.col], 0
	jmp update_cursor

newline:
	call vertical_tab
	jmp carriage_return

print_chr:
	cmp dil, 10
	je newline
	cmp dil, 11
	je vertical_tab
	cmp dil, 13
	je carriage_return
	mov al, [pos.col]
	inc al
	cmp al, 80
	je .newline
	mov [pos.col], al
	jmp set_chr
.newline:
	push rdi
	call newline
	pop rdi
	jmp set_chr

print_str:
	mov rax, rdi
.print:
	mov dil, [rax]
	cmp dil, 0
	je .return
	push rax
	call print_chr
	pop rax
	inc rax
	jmp .print
.return:
	ret

print_hex:
	mov rsi, 0x10
	jmp print_num
print_dec:
	mov rsi, 10
print_num:
	mov rax, rdi
	xor rcx, rcx
.convert:
	inc rcx
	xor rdx, rdx
	div rsi
	cmp dl, 10
	jb .digit
	add dl, 'A'-10
	jmp .next
.digit:
	add dl, '0'
.next:
	push rdx
	cmp rax, 0
	jne .convert
.print:
	cmp rcx, 0
	je .return
	dec rcx
	pop rdi
	push rcx
	call print_chr
	pop rcx
	jmp .print
.return:
	ret

clear_screen:
	push rbx
	push r12
	push r13
	push r14
	push r15

	mov qword[cursor], 0xB8000
.clr:
	cmp qword[cursor], 0xB8FA0
	je .return
	mov dil, ' '
	call set_chr
	jmp .clr
.return:
	mov qword[cursor], 0xB8000
	mov byte[pos.row], 0
	mov byte[pos.col], 0
	call update_cursor

	pop r15
	pop r14
	pop r13
	pop r12
	pop rbx
	ret

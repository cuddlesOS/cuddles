global letters

%define GFXINFO 0x1000-10
%define PITCH GFXINFO+0
%define WIDTH GFXINFO+2
%define HEIGHT GFXINFO+4
%define FRAMEBUFFER GFXINFO+6

section .text

; str in rdi
legacy_debug:
	mov r9, [line]

	mov rax, rcx
	mov rbx, 15
	xor rdx, rdx
	mul rbx

	cmp rax, [HEIGHT]
	jmp .char

	xor rax, rax
	xor r9, r9

.char:
	xor rax, rax

	mov al, [rdi]
	cmp al, 0
	je .return

	cmp al, ' '
	je .space

	cmp al, 'a'
	jb .invalid

	cmp al, 'z'
	ja .invalid

	sub al, 'a'
	mov bl, 15
	mul bl

	add rax, letters
	mov r8, rax

	jmp .render

.space:
	mov r8, letters.space

.render:

.target:
	xor rdx, rdx
	mul [PITCH]

	xor rbx, rbx
	mov ebx, [FRAMEBUFFER]
	add rax, rbx

	inc rdi
	jmp debug

.return:
	ret

.invalid:
	mov rdi, .invalid_msg
	call debug
	jmp $

.invalid_msg: "invalid character in message", 0

section .data

line: dq 0

letters:

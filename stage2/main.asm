[org 0x7E00]

%define PAGETABLE 0x1000
%define VESAINFO  0x0500
%define VESAMODE VESAINFO+512
%define OWNMODE  VESAMODE+256
%define GFXINFO PAGETABLE-10
;%define MEMMAPCNT GFXINFO-2
%define MEMMAP 0x500

setup:
	; print message
	mov ebx, .msg
	call print_str

	; setup VESA
	call vesa

	; get extended memory map
	call mmap

	; build page table
	call paging

	; jump into long mode
	jmp 0x0008:long_mode

.msg:
	db 10, 13, "nyax stage2", 10, 13, 0

%include "stage2/vesa.asm"
%include "stage2/mmap.asm"
%include "stage2/paging.asm"
%include "stage1/print.asm"

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

[bits 64]

long_mode:
	; setup segment registers
	mov ax, 0x0010
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	; kernel begins here

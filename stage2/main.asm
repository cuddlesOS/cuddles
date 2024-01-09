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
	db 10, 13, "cuddles stage2", 10, 13, 0

%include "stage2/vesa.asm"
%include "stage2/mmap.asm"
%include "stage2/paging.asm"
%include "stage1/print.asm"

[bits 64]

long_mode:
	; setup segment registers
	mov ax, 0x0010
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	mov eax, [kernel_elf]
	cmp eax, [.elf]
	jne .fail

	mov r8, [kernel_elf+32] ; program header pos
	add r8, kernel_elf

	movzx rbx, word[kernel_elf+54] ; program header size
	movzx rdx, word[kernel_elf+56] ; num of program headers

.header:
	cmp rdx, 0
	je .start

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

.start:
	; more stack space
	mov rsp, 0x80000
	xor rbp, rbp

	; ELF entry point
	mov rax, [kernel_elf+24]
	call rax

.fail:
	cli
	hlt
	jmp $

.elf: db 0x7f, "ELF"

kernel_elf:

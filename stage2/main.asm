[org 0x7E00]

%define PAGETABLE 0x1000
%define VESAMODES 0x500
%define MEMMAP 0x500

setup:
	mov [bootinfo.ksize], edi

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

bootinfo:
	.ksize: dq 0
	.gfx_pitch: dw 0
	.gfx_width: dw 0
	.gfx_height: dw 0
	.gfx_framebuffer: dq 0
	.mmap_len: dq 0
	.mmap_ptr: dq 0

%include "stage2/vesa.asm"
%include "stage2/mmap.asm"
%include "stage2/paging.asm"
%include "stage1/print.asm"

[bits 64]

%include "stage2/elf.asm"

long_mode:
	; setup segment registers
	mov ax, 0x0010
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	call load_kernel_elf ; returns entry point in rax

	; more stack space
	mov rsp, 0x80000
	xor rbp, rbp

	; pass bootinfo as arg
	mov rdi, bootinfo
	call rax

kernel_elf:

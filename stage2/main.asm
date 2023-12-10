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

	; kernel begins here

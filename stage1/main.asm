[org 0x7C00]

%define KSTART 0x7E00
%define KSECTORS (KSIZE + 511) / 512

boot:
	; init segment registers
	xor ax, ax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	; init stack
	mov bp, KSTART ; stack grows down, overwriting MBR
	mov sp, bp

	; print message
	mov ebx, .msg
	call print_str

	; print boot drive
	pusha
	movzx eax, dl
	call print_hex
	call newline
	popa

	; load stage2 and stage3
	call load_stages

	; jump into stage2
	jmp KSTART

.msg: db 10, 13, "nyax stage1", 10, 13, "boot drive: 0x", 0

load_stages:
	mov ebx, .msg
	call print_str

	mov ah, 0x02     ; read sectors from drive
	mov al, KSECTORS ; number of sectors
	xor ch, ch       ; cylinder=0
	mov cl, 2        ; sector=2
	xor dh, dh       ; head=0
	mov bx, KSTART   ; buffer
	int 0x13
	jc .fail         ; CF set on error
	cmp al, KSECTORS ; check read sectors count
	jne .fail

	ret

.fail:
	mov ebx, .fail_msg
	call print_str
	hlt

.msg: db "loading stage2 and stage3 from disk", 10, 13, 0
.fail_msg: db "disk failure, try rebooting", 10, 13, 0

%include "stage1/print.asm"

times 510-($-$$) db 0
dw 0xAA55

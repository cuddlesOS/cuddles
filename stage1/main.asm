[org 0x7C00]

%define KSTART 0x7E00

boot:
	; init segment registers
	xor ax, ax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	; init stack
	mov bp, 0x7C00
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

	; reclaim MBR space for stack
	mov bp, KSTART
	mov sp, bp

	; jump into stage2
	jmp KSTART

.msg: db 10, 13, "nyax stage1", 10, 13, "boot drive: 0x", 0

load_stages:
	mov ebx, .msg
	call print_str

	; mov si, 18 ; sectors per track
	; mov di, 2  ; number of heads
	; check if hard drive
	; test dl, 0x80
	; jz .start_load ; use defaults if not a hard drive

	; backup dl
	push dx

	; get geometry
	mov ah, 8
	int 0x13

	; restore dl
	pop ax
	mov dl, al

	movzx si, cl
	and si, 0x3f

	movzx di, dh
	inc di

.start_load:
	pusha

	mov ebx, .sectors
	call print_str
	movzx eax, si
	call print_dec
	call newline

	mov ebx, .heads
	call print_str
	movzx eax, di
	call print_dec
	call newline

	popa

	; setup buffer
	mov ax, KSTART/0x10
	mov es, ax
	mov bx, 0

	mov cl, 2       ; sector=2
	mov dh, 0       ; head=0
	mov ch, 0       ; cylinder=0

.load:
	mov ah, 0x02    ; read sectors from drive
	mov al, 1       ; number of sectors
	int 0x13
	jc .fail        ; CF set on error

	; check read sectors count
	cmp al, 1
	jne .fail

	; increase buffer pointer
	add bx, 512

	; check if finished
	cmp bx, KSIZE
	jae .success

	; next sector
	inc cl
	movzx ax, cl
	cmp ax, si
	jbe .load

	; next head
	mov cl, 1
	inc dh
	movzx ax, dh
	cmp ax, di
	jb .load

	; next cylinder
	mov dh, 0
	inc ch
	jmp .load

.success:
	; restore es
	mov ax, 0
	mov es, ax

	ret

.fail:
	mov ebx, .fail_msg
	call print_str
	hlt

.msg: db "loading stage2 and stage3 from disk", 10, 13, 0
.fail_msg: db "disk failure, try rebooting", 10, 13, 0
.sectors: db "sectors per track: ", 0
.heads: db "number of heads: ", 0

%include "stage1/print.asm"

times 510-($-$$) db 0
dw 0xAA55

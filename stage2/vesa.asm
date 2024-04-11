vesainfo: times 512 db 0
vesamode: times 256 db 0

vesa:
	; print message
	mov ebx, .msg
	call print_str

	; get vesa bios info
	mov eax, dword[.vbe2]
	mov dword[vesainfo], eax ; move "VBE2" to start of vesainfo struct
	mov ax, 0x4F00           ; get VESA BIOS information
	mov di, vesainfo         ; struct buffer
	int 0x10

	cmp ax, 0x004F           ; check ax for correct magic number
	jne .fail_getinfo

	mov eax, dword[.vesa]
	cmp dword[vesainfo], eax ; check if "VESA" is at start of struct
	jne .fail_getinfo

	; print select message
	mov ebx, .select_msg
	call print_str

	; get segment:offset pointer to video modes into gs:ebx
	movzx ebx, word[vesainfo+14]
	mov    ax, word[vesainfo+16]
	mov gs, ax

	; convert modes to own structure

	xor esi, esi        ; number of avail modes

.mode_loop:
	; get mode info
	mov cx, [gs:ebx]    ; video mode into cx
	cmp cx, 0xFFFF      ; 0xFFFF is terminator, no suitable mode has been found
	je .mode_done
	mov ax, 0x4F01      ; get VESA mode information
	mov di, vesamode    ; vesa mode info struct buffer
	int 0x10

	cmp ax, 0x004F      ; check ax for correct magic number
	jne .fail_modeinfo

	mov al, byte[vesamode] ; get attributes
	and al, 0b10000000     ; extract bit 7, indicates linear framebuffer support
	jz .mode_next

	mov al, byte[vesamode+25] ; get bpp (bits per pixel)
	cmp al, 32
	jne .mode_next

	push ebx ; print_dec and print_str modify ebx

	mov eax, esi
	mov ebx, 12
	mul ebx
	mov edi, eax
	add edi, VESAMODES

	mov [edi+10], cx ; copy mode

	; print selector
	mov al, '['
	call print_chr

	mov eax, esi
	add eax, 'a'
	call print_chr

	mov al, ']'
	call print_chr

	mov al, ' '
	call print_chr

	mov ax, [vesamode+16] ; copy pitch
	mov [edi+0], ax

	movzx eax, word[vesamode+18] ; copy width
	mov [edi+2], ax
	call print_dec

	mov al, 'x'
	call print_chr

	movzx eax, word[vesamode+20] ; copy height
	mov [edi+4], ax
	call print_dec
	call newline

	mov eax, [vesamode+40] ; copy framebuffer
	mov [edi+6], eax

	pop ebx

	inc esi
	cmp esi, 'z'-'a'   ; only print up to z
	jg .mode_done

.mode_next:
	add ebx, 2         ; increase mode pointer
	jmp .mode_loop     ; loop

.mode_done:
	cmp esi, 0
	je .fail_nomode

.input:
	mov ebx, .select_prompt
	call print_str

	mov ah, 0x00   ; get keypress, blocking
	int 0x16

	call print_chr ; echo user input

	movzx edi, al  ; backup al
	call newline

	sub edi, 'a'
	cmp edi, esi
	jb .valid      ; check validity

	mov ebx, .invalid
	call print_str

	jmp .input

.valid:
	; convert selected number to address
	mov eax, edi
	mov ebx, 12
	mul ebx
	add eax, VESAMODES

	; copy to final gfx info location
	mov bx, [eax+0]
	mov [bootinfo.gfx_pitch], bx

	mov bx, [eax+2]
	mov [bootinfo.gfx_width], bx

	mov bx, [eax+4]
	mov [bootinfo.gfx_height], bx

	mov ebx, [eax+6]
	mov dword[bootinfo.gfx_framebuffer+0], ebx
	mov dword[bootinfo.gfx_framebuffer+4], 0

	;mov edi, eax
	;mov eax, [edi+6]
	;call print_hex
	;call newline
	;mov eax, edi
	;jmp $

	;ret

	; set mode
	mov bx, [eax+10]           ; video mode in bx (first 13 bits)
	or  bx, 1 << 14            ; set bit 14: enable linear frame buffer
	and bx, 0b0111111111111111 ; clear deprecated bit 15
	mov ax, 0x4F02             ; set VBE mode
	int 0x10

	ret

.msg: db "setting up vesa", 10, 13, 0
.vbe2: db "VBE2"
.vesa: db "VESA"
.select_msg: db "avaliable video modes:", 10, 13, 0
.select_prompt: db "select video mode: ", 0
.invalid: db "invalid input", 10, 13, 0

.fail_getinfo:
	mov ebx, .fail_getinfo_msg
	jmp .fail

.fail_modeinfo:
	mov ebx, .fail_modeinfo_msg
	jmp .fail

.fail_nomode:
	mov ebx, .fail_nomode_msg
	jmp .fail

.fail_getinfo_msg: db "failed getting vesa bios info", 10, 13, 0
.fail_modeinfo_msg: db "failed getting video mode info", 10, 13, 0
.fail_nomode_msg: db "no suitable video modes available", 10, 13, 0

.fail:
	call print_str
	jmp $

global page_region
extern print_hex, print_chr, newline, print_dec, print_str

section .text

pagebuf_init:
	.start: dq 0x5000
	.size: dq 0x2000
	.used: dq 0

pagebuf: dq pagebuf_init

next_page: dq 0

; allocate new page table buffer
alloc:
	; rsi = buffer (result)
	; rdi = next_page
	; r8  = pagebuf
	; rbx = upper
	; rax = tmp; used_next

	mov r8, [pagebuf] ; *pagebuf
	mov rsi, [r8]     ; start = pagebuf->start

	mov rbx, [r8+8]   ; size = pagebuf->size
	add rbx, rsi      ; upper_have = start + size

	; round *up* to 0x1000 align
	mov rax, 0xfff
	add rsi, rax
	not rax
	and rsi, rax      ; aligned_start = (start + 0xfff) & (~0xfff)

	mov rax, [r8+16]  ; used = pagebuf->used
	add rax, 0x1000
	add rsi, rax      ; upper_need = aligned_start + used + 0x1000

	cmp rsi, rbx      ; if upper_need > upper_have
	ja .newbuf        ; current region is full, get new

	cmp rsi, r10      ; if upper_need >= next_page
	jae .oom          ; out of memory (target buffer isn't paged yet)

	mov [r8+16], rax  ; pagebuf->used = used + 0x1000

	; clear out buffer

	mov rbx, rsi
	sub rsi, 0x1000

.clear:
	sub rbx, 8
	mov qword[rbx], 0
	cmp rbx, rsi
	jne .clear

	ret

; select next page buffer
.newbuf:
	cmp r8, pagebuf_init
	jne .nextbuf

	mov r8, 0x500
	jmp .trybuf

.nextbuf:
	add r8, 24

.trybuf:
	cmp qword[r8], 0
	je .oom       ; last region reached

	mov rax, [r8+16]

	cmp rax, -1
	je .nextbuf   ; region is reserved

	cmp rax, 0
	jne .oom      ; region has not been paged yet

	mov [pagebuf], r8
	jmp alloc

.oom:
	push rdi

	mov rdi, .oom_msg
	call print_str

	pop rdi

	call print_hex
	call newline

	jmp $

.oom_msg: db "out of memory for page table", 10, "next_page = ", 0

; get/create page tables
get_tables:
; level 4

	; rdi = address         (arg, persist)
	; rax = tmp
	; rbx = mask
	; rcx = bits            (persist)
	; rdx = level           (persist)
	; r8  = table address
	; rsi = next offset     (persist)

	mov cl, 12+9*4
	mov dl, 4

	mov rsi, 0x1000

; level 4
.level:
	dec dl
	mov r8, rdi
	mov rbx, -1           ; reset remainder mask
	shl rbx, cl           ; update remainder mask
	not rbx               ; negate remainder mask
	and r8, rbx           ; apply remainder mask

	mov al, 9
	mul dl
	add al, 12
	mov cl, al

	shr r8, cl            ; divide
	shl r8, 3             ; multiply by 8

	mov rbx, 0xfff        ; 0x1000 alignment
	not rbx               ; offset mask

	and rsi, rbx          ; apply offset mask
	add r8, rsi           ; add offset
	push r8               ; store

	cmp dl, 0
	je .done

	mov rsi, [r8]         ; next offset
	cmp rsi, 0
	jne .level

	call alloc
	or rsi, 3
	mov r8, [rsp]
	mov [r8], rsi

	jmp .level

.done:
	pop r11
	pop r12
	pop r13
	pop r14

	ret

space:
	mov dil, ' '
	jmp print_chr

page_region:
	push rbx
	push r12
	push r13
	push r14
	push r15

	mov r9, rdi

	mov rdi, [r9]   ; ptr = mmap_entry->ptr
	mov r10, [next_page]

	push rdi

	mov rax, 1 << 63

	or rdi, rax
	call print_hex
	call space

	mov rdi, [r9+8]
	add rdi, [rsp]
	or rdi, rax

	call print_hex
	call newline

	pop rdi

	; for   usable region (type = 1), set mmap_entry->used =  0
	; for reserved region (type = 2), set mmap_entry->used = -1
	xor rax, rax
	xor rbx, rbx
	mov eax, dword[r9+16]
	cmp rax, 1
	je .set_used
	dec rbx
.set_used:
	mov [r9+16], rbx

	mov r10, rdi
	mov r15, rdi    ; r15 = end of region
	add r15, [r9+8]

	mov rax, 0xfff
	not rax
	and rdi, rax    ; round down to 0x1000 aligned

	cmp rdi, r10
	jb .get_tables

	mov r10, rdi

.get_tables:
	call get_tables ; page tables into r11-r14

	; start filling L1 map
.l1:
	mov rax, rdi
	or rax, 3
	mov [r11], rax

	add rdi, 0x1000

	cmp rdi, r10
	jb .next

	mov r10, rdi

.next:
	cmp rdi, r15    ; if next >= end
	jae .done

	; prepare rcx mask for later
	mov rcx, -1
	shl rcx, 3

	; bump L1

	add r11, 8
	mov rax, r11
	and rax, 0xfff
	jnz .l1

	; bump L2

	add r12, 8
	mov rax, r12
	and rax, 0xfff
	jnz .l2

	; bump L3

	add r13, 8
	mov rax, r13
	and rax, 0xfff
	jnz .l3

	; bump L4

	add r14, 8
	mov rax, r14
	and rax, 0xfff
	jnz .l4

	; machine has more than 256TB of RAM, tell user to fuck off
	jmp .bruh

.l4:
	mov r13, [r14]
	and r13, rcx
	jnz .l3

	call alloc
	mov r13, rsi
	or rsi, 3
	mov [r14], rsi

.l3:
	mov r12, [r13]
	and r12, rcx
	jnz .l2

	call alloc
	mov r12, rsi
	or rsi, 3
	mov [r13], rsi

.l2:
	mov r11, [r12]
	and r11, rcx
	jnz .l2

	call alloc
	mov r11, rsi
	or rsi, 3
	mov [r12], rsi

	jmp .l1

.done:
	mov [next_page], r10

	pop r15
	pop r14
	pop r13
	pop r12
	pop rbx

	ret

.bruh:
	mov rdi, .bruh_msg
	call print_str
	jmp $

.bruh_msg: db "bruh why do you have more than 256TB of RAM (you fucking glow)", 10, 0

; identity map available memory
old_page_map:
	mov r9, 0x0500                ; mmap_entry
.entry:
	cmp qword[r9], 0
	je .done
	call page_region
	add r9, 24
	jmp .entry
.done:
	ret

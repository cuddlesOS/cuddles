paging:
	; print message
	mov ebx, .msg
	call print_str

	; clear 4 levels of page maps
	mov di, PAGETABLE+0x0000
.clr_buf:
	mov byte[di], 0
	inc di
	cmp di, PAGETABLE+0x5000
	jne .clr_buf

	; init 3 page map levels
	mov dword[PAGETABLE+0x0000], PAGETABLE+0x1003
	mov dword[PAGETABLE+0x1000], PAGETABLE+0x2003
	mov dword[PAGETABLE+0x2000], PAGETABLE+0x3003
	mov dword[PAGETABLE+0x2008], PAGETABLE+0x4003

	; fill up level 4 page map
	mov eax, 3
	mov di, PAGETABLE+0x3000
.build_pt:
	mov [di], eax
	add di, 8
	add eax, 0x1000
	cmp eax, 0x400000
	jb .build_pt

	; enable paging and long mode

	mov di, PAGETABLE

	; mask all IRQs
	mov al, 0xFF
	out 0xA1, al
	out 0x21, al

	nop
	nop

	lidt [.idt]

	mov eax, 0b10100000
	mov cr4, eax

	mov edx, edi
	mov cr3, edx

	mov ecx, 0xC0000080
	rdmsr
	or eax, 0x00000100
	wrmsr

	mov ebx, cr0
	or ebx, 0x80000001
	mov cr0, ebx

	lgdt [.gdt_pointer]

	ret

.gdt:
	dq 0
	dq 0x00209A0000000000
	dq 0x0000920000000000
	dw 0

.gdt_pointer:
	dw $ - .gdt - 1
	dd .gdt

.idt:
	dw 0
	dd 0

.msg:
	db "building page table", 10, 13, 0

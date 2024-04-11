%define MAPMAGIC 0x534D4150

mmap:
	mov ebx, .msg
	call print_str

	xor ebx, ebx    ; counter for interrupt
	mov di, MEMMAP
	xor esi, esi    ; number of regions

.loop:
	; issue an INT 0x15, EAX = 0xE820 interrupt
	mov eax, 0xE820
	mov ecx, 24
	mov edx, MAPMAGIC
	int 0x15

	cmp eax, MAPMAGIC ; detect failure
	jne .fail

	inc esi
	add di, 24

	cmp ebx, 0
	jne .loop

	mov dword[di+0], 0
	mov dword[di+4], 0

	mov dword[bootinfo.mmap_len+0], esi
	mov dword[bootinfo.mmap_len+4], 0
	mov dword[bootinfo.mmap_ptr+0], MEMMAP
	mov dword[bootinfo.mmap_ptr+4], 0

	ret

.fail:
	mov ebx, .fail_msg
	call print_str
	jmp $

.msg: db "getting extended memory map", 10, 13, 0
.fail_msg: db "memory map failure", 10, 13, 0


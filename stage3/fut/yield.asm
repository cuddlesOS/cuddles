global process_yield
extern rc_drop

%define CURRENT [0x500]

section .text
process_yield:
	; save callee reserved regs to old stack
	push rbx
	push rbp
	push r12
	push r13
	push r14
	push r15

	; get CURRENT
	mov rdi, CURRENT

	; set CURRENT->stack_ptr
	mov rax, rdi
	add rax, 16
	mov [rax], rsp

	; switch to new process
	add rax, 8
	mov rbx, [rax]
	mov CURRENT, rbx

	; activate new stack
	add rbx, 16
	mov rsp, [rbx]

	; drop old process (=rdi)
	call rc_drop

	; grab new process
	mov rdi, CURRENT
	call rc_grab

	; restore callee reserved registers
	pop r15
	pop r14
	pop r13
	pop r12
	pop rbp
	pop rbx

	; jump into new process, with style
	ret

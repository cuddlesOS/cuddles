print("global idt_entries")
print("extern interrupt_handler")

local has_error_code = {}
for _, x in pairs({8, 10, 11, 12, 13, 14, 17, 21, 29, 30}) do
	has_error_code[x] = true
end

print("section .data")
print("idt_entries:")
for i = 0, 255 do
	print("dq isr_"..i)
end

print("section .text")
for i = 0, 255 do
	print("isr_" .. i .. ":")
	print("cli")

	if not has_error_code[i] then
		print("push 0")
	end

	print("push " .. i)
	print("jmp isr_common")
end

print([[
isr_common:
	push r15
	push r14
	push r13
	push r12
	push r11
	push r10
	push r9
	push r8
	push rsi
	push rdi
	push rbp
	push rdx
	push rcx
	push rbx
	push rax

	cld
	xor rbp, rbp
	mov rdi, rsp
	call interrupt_handler

	pop rax
	pop rbx
	pop rcx
	pop rdx
	pop rbp
	pop rdi
	pop rsi
	pop r8
	pop r9
	pop r10
	pop r11
	pop r12
	pop r13
	pop r14
	pop r15

	add rsp, 16

	iretq
]])

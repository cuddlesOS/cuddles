print("global idt_entries")
print("extern interrupt_handler")

local has_error_code = {}
for _, x in pairs({8, 10, 11, 12, 13, 14, 17, 21, 29, 30}) do
	has_error_code[x] = true
end

for i = 0, 255 do
	print("isr_" .. i .. ":")
	print("cli")

	if not has_error_code[i] then
		print("push 0")
	end

	print("push " .. i)
	print("jmp isr_common")
end

print("idt_entries:")
for i = 0, 255 do
	print("dq isr_"..i)
end

print([[
isr_common:
	push   r11
	push   r10
	push   r9
	push   r8
	push   rdi
	push   rsi
	push   rcx
	push   rdx
	push   rax

	cld
	mov rdi, rsp
	call interrupt_handler

	pop    rax
	pop    rdx
	pop    rcx
	pop    rsi
	pop    rdi
	pop    r8
	pop    r9
	pop    r10
	pop    r11

	add rsp, 16

	iretq
]])

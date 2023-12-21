local regs = {
	"rax", "rbx",
	"rcx", "rdx",
	"rdi", "rsi",
	"rbp", "rsp",
	"r8", "r9",
	"r10", "r11",
	"r12", "r13",
	"r14", "r15",
}

print("global watchdog")
print("extern rand, watchdog_err")

print("regs_backup: resq 16")
print("regs_rand: resq 16")

print("watchdog:")

local function iter_regs(f)
	for i, r in ipairs(regs) do
		f(i-1, r)
	end
end

-- backup registers
iter_regs(function(i, r) print("mov [regs_backup+" .. (8*i) .. "], " .. r) end)

-- populate random table
iter_regs(function(i) print("xor rax, rax\ncall rand\nmov [regs_rand+" .. (8*i) .. "], rax") end)

-- read registers from random table
iter_regs(function(i, r)
	if r == "rsp" then
		print("mov [regs_rand+" .. (8*i) .. "], rsp")
	else
		print("mov " .. r .. ", [regs_rand+" .. (8*i) .. "]")
	end
end)

print(".loop:")

-- wait for interrupts
print("hlt")

-- check and jump if changed
iter_regs(function(i, r)
	print("cmp " .. r .. ", [regs_rand+" .. (8*i) .. "]")
	print("jne .ch_" .. r)
end)

print("jmp .loop")

-- write error and return
iter_regs(function(i, r)
	print(".ch_" .. r .. ":")
	print("mov qword[watchdog_err], " .. i)
	print("mov qword[watchdog_err+8], " .. r)
	print("mov rax, [regs_rand+" .. (8*i) .. "]")
	print("mov qword[watchdog_err+16], rax")
	print("jmp .ret")
end)

print(".ret:")

-- restore registers
iter_regs(function(i, r)
	if r ~= "rsp" then
		print("mov " .. r .. ", [regs_backup+" .. (8*i) .. "]")
	end
end)

print("ret")

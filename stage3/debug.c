#include "fs.h"
#include "font.h"
#include "string.h"
#include "halt.h"
#include "debug.h"

static str dbg_disas = NILS;

static str exception[32] = {
	S("Division Error"),
	S("Debug"),
	S("Non-maskable Interrupt"),
	S("Breakpoint"),
	S("Overflow"),
	S("Bound Range Exceeded"),
	S("Invalid Opcode"),
	S("Device Not Available"),
	S("Double Fault"),
	S("Coprocessor Segment Overrun"),
	S("Invalid TSS"),
	S("Segment Not Present"),
	S("Stack-Segment Fault"),
	S("General Protection Fault"),
	S("Page Fault"),
	NILS,
	S("x87 Floating-Point Exception"),
	S("Alignment Check"),
	S("Machine Check"),
	S("SIMD Floating-Point Exception"),
	S("Virtualization Exception"),
	S("Control Protection Exception"),
	NILS, NILS, NILS, NILS, NILS, NILS,
	S("Hypervisor Injection Exception"),
	S("VMM Communication Exception"),
	S("Security Exception"),
	NILS,
};

static bool get_source(u64 code, str *func, str *linep)
{
	if (dbg_disas.data == nil)
		return false;

	*func = (str) NILS;
	*linep = (str) NILS;

	str iter = dbg_disas;
	while (iter.len > 0) {
		str line = str_walk(&iter, S("\n"));
		u64 func_addr;
		usize adv;
		if ((adv = str_parse_num(line, 16, &func_addr))) {
			line = str_advance(line, adv);
			if (line.len < 2 || line.data[0] != ' ' || line.data[1] != '<')
				continue;
			line = str_advance(line, 2);
			usize close = str_find(line, S(">"));
			*func = (str) { .len = close, .data = line.data };
		} else {
			u64 addr;
			if (!str_parse_num(str_eat(line, S(" ")), 16, &addr))
				continue;
			*linep = line;
			if (addr == code)
				return true;
		}
	}

	return false;
}

static void trace(u64 code, u64 base)
{
	if (base == 0)
		return;

	str func, line;
	if (get_source(code, &func, &line)) {
		print(S("\t"));
		print_hex(code);
		print(S(" in "));
		print(func);
		print(S("\n"));
	}

	u64 *stack = (u64 *) base;
	trace(stack[1], stack[0]);
}

static void dump_frame(interrupt_frame *frame)
{
	str func, line;
	if (get_source(frame->rip, &func, &line)) {
		print(line);
		print(S("\n"));
	}

	print(S("backtrace:\n"));
	trace(frame->rip, frame->rbp);

	print(S("rip = "));
	print_hex(frame->rip);
	print(S("\n"));

	struct {
		u8 bit;
		str name;
	} flags[] = {
		{  0, S("CF")  },
		{  2, S("PF")  },
		{  4, S("AF")  },
		{  6, S("ZF")  },
		{  7, S("SF")  },
		{  8, S("TF")  },
		{  9, S("IF")  },
		{ 10, S("DF")  },
		{ 11, S("OF")  },
		{ 16, S("RF")  },
		{ 17, S("VM")  },
		{ 18, S("AC")  },
		{ 19, S("VIF") },
		{ 20, S("ID")  },
	};

	print(S("rflags = "));

	usize f = 0;
	for (usize i = 0; i < 63; i++) {
		bool has_name = f < LEN(flags) && flags[f].bit == i;

		if (frame->rflags & ((u64) 1 << i)) {
			if (has_name)
				print(flags[f].name);
			else
				print_dec(i);
			print(S(" "));
		}

		if (has_name)
			f++;
	}

	print(S("\n"));

#define REG(X) print(S(#X)); if (S(#X).len == 2) print(S(" ")); print(S(" = ")); print_num_pad(frame->X, 16, 16, ' ');
#define REGS(A, B) REG(A) print(S("    ")); REG(B) print(S("\n"));

	REGS(rax, rbx)
	REGS(rcx, rdx)
	REGS(rsp, rbp)
	REGS(rdi, rsi)
	REGS(r8,  r9)
	REGS(r10, r11)
	REGS(r12, r13)
	REGS(r14, r15)
	REGS(ss, cs)

#undef REG
#undef REGS
}

void debug_exception(interrupt_frame *frame)
{
	// TODO: possible race condition due to printing here
	// when exception happens in printing code itself

	if (exception[frame->which].data == nil) {
		print(S("Unknown Exception "));
		print_dec(frame->which);
	} else {
		print(exception[frame->which]);
	}
	print_char('\n');

	if (frame->which == 14) {
		str bits[8] = {
			S("present"),
			S("write"),
			S("user"),
			S("reserved_write"),
			S("instruction_fetch"),
			S("protection_key"),
			S("shadow_stack"),
			S("software_guard_extensions"),
		};

		u8 err = frame->error_code;

		for (int i = 0; i < 8; i++) {
			print(bits[i]); print(S(" = ")); print((err & 1) ? S("true\n") : S("false\n"));
			err >>= 1;
		}
	} else {
		print(S("error_code = ")); print_dec(frame->error_code); print_char('\n');
	}

	dump_frame(frame);
	freeze();
}

void debug_init()
{
	print(S("loading kernel debug info... "));
	dbg_disas = fs_read(S("dbg/kernel.dis.asm"));
	print(S("done\n"));
}

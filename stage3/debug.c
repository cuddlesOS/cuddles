#include "fs.h"
#include "font.h"
#include "string.h"
#include "halt.h"
#include "debug.h"

static str dbg_map = NILS;
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

static void dump_frame(interrupt_frame *frame)
{
	print(S("rip = "));
	print_hex(frame->rip);

	if (dbg_map.data != nil) {
		str entry = NILS;
		str iter = dbg_map;
		while (iter.len > 0) {
			str line = str_walk(&iter, S("\n"));
			line = str_eat(line, S(" "));
			if (!str_start(line, S("0x")))
				continue;
			line = str_advance(line, 2);

			u64 addr;
			usize adv = str_parse_num(line, 16, &addr);
			if (adv == 0)
				continue;
			line = str_advance(line, adv);

			if (addr > frame->rip)
				break;

			line = str_eat(line, S(" "));
			if (line.len > 0)
				entry = line;
		}

		if (entry.len > 0) {
			print(S(" in "));
			print(entry);
		}
	}

	if (dbg_disas.data != nil) {
		str iter = dbg_disas;
		while (iter.len > 0) {
			str line = str_walk(&iter, S("\n"));
			u64 addr;
			if (!str_parse_num(str_eat(line, S(" ")), 16, &addr))
				continue;
			if (addr == frame->rip) {
				print(S("\n"));
				print(line);
				break;
			}
		}
	}

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
	print(S("loading kernel debug info...\n"));
	dbg_map = fs_read(S("dbg/kernel.map"));
	dbg_disas = fs_read(S("dbg/kernel.dis.asm"));
}

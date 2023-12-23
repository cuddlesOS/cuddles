#include "shell.h"
#include "font.h"
#include "heap.h"
#include "fs.h"
#include "gfx.h"
#include "string.h"
#include "pci.h"
#include "memory.h"
#include "io.h"
#include "math.h"
#include "clock.h"
#include "thread.h"
#include "rng.h"
#include "cheese_demo.h"

static void cmd_echo(str arg)
{
	print(arg);
	print_char('\n');
}

static void cmd_cat(str arg)
{
	str f = fs_read(arg);

	if (f.data == nil) {
		print(S("cat: file not found: "));
		print(arg);
		print(S("\n"));
	} else {
		print(f);
		free(f.data);
	}
}

static void cmd_font(str arg)
{
	str f = fs_read(arg);

	if (f.data == nil) {
		print(S("font: file not found: "));
		print(arg);
		print(S("\n"));
	} else {
		if (f.len == 16*256)
			font_load_blob(f.data);
		else
			print(S("font: invalid file size\n"));

		free(f.data);
	}
}

static void cmd_fontdemo()
{
	const u8 max = '~' - '!' + 1;
	char buf[max];

	for (u8 i = 0; i < max; i++)
		buf[i] = i + '!';

	print(S("fontdemo:\n"));
	print((str) { max, buf });
	print(S("\n"));
}

static void cmd_img(str arg)
{
	str f = fs_read(arg);

	if (f.data == nil) {
		print(S("img: file not found: "));
		print(arg);
		print(S("\n"));
	} else {
		if (f.len < 2 * sizeof(u32))
			print(S("img: missing header\n"));
		else {
			u32 width = ((u32 *) f.data)[0];
			u32 height = ((u32 *) f.data)[1];

			if (f.len != 2 * sizeof(u32) + width * height * sizeof(color))
				print(S("img: invalid file size\n"));
			else
				gfx_draw_img(gfx_info->width-width, 0, width, height,
					(void *) (f.data + 2 * sizeof(u32)));
		}

		free(f.data);
	}
}

static void cmd_lspci(str arg)
{
	(void) arg;
	pci_enumerate();
}

static void cmd_run(str arg)
{
	str f = fs_read(arg);

	if (f.data == nil) {
		print(S("run: file not found: "));
		print(arg);
		print(S("\n"));
	} else {
		str iter = f;
		for (;;) {
			str cmd = str_walk(&iter, S("\n"));
			if (cmd.data == nil)
				break;
			shell_run_cmd(cmd);
		}

		free(f.data);
	}
}

extern char keymap[256];

static void cmd_loadkeys(str arg)
{
	str f = fs_read(arg);

	if (f.data == nil) {
		print(S("loadkeys: file not found: "));
		print(arg);
		print(S("\n"));
	} else {
		if (f.len == 256)
			memcpy(keymap, f.data, 256);
		else
			print(S("loadkeys: invalid file size\n"));

		free(f.data);
	}
}

static void cmd_clear(str arg)
{
	(void) arg;
	font_clear_screen();
}

static void cmd_love(str arg)
{
	if (arg.len == 0)
		arg = (rand()%2) ? S("anna") : S("floof");

	str f = fs_read(S("uwu.txt"));
	if (f.data == nil) {
		print(S("love: missing file uwu.txt\n"));
	} else {
		isize start = 172-arg.len/2;
		if (start < 0 || start + arg.len > f.len) {
			print(S("love: argument too long (owo it's too big for me)\n"));
		} else {
			memcpy(f.data+start, arg.data, arg.len);
			print(f);
		}

		free(f.data);
	}
}

extern str version;

static void cmd_uname(str arg)
{
	(void) arg;
	print(S("cuddles "));
	print(version);
	print(S("\n"));
}

static void print_bytes(usize bytes)
{
	static char fmt[] = { ' ', 'K', 'M', 'G', 'T' };
	usize unit = ipow(1000, LEN(fmt)-1);
	for (usize i = 0; i < LEN(fmt); i++) {
		if (bytes >= unit || unit == 1) {
			print_num_pad(bytes/unit, 10, 3, ' ');
			print_char('.');
			print_dec((bytes%unit)/100);
			print_char(' ');
			print_char(fmt[LEN(fmt)-1-i]);
			print_char('B');
			break;
		}
		unit /= 1000;
	}
}

static void cmd_ls(str arg)
{
	dir d = fs_readdir(arg);
	usize longest = 0;
	for (usize i = 0; i < d.len; i++)
		if (longest < d.data[i].name.len)
			longest = d.data[i].name.len;
	for (usize i = 0; i < d.len; i++) {
		print_char(' ');
		print_bytes(d.data[i].size);
		print_char(' ');
		print(d.data[i].name);
		for (usize j = 0; j < longest+1-d.data[i].name.len; j++)
			print_char(' ');
		if (d.data[i].is_dir) {
			print_dec(d.data[i].children);
			print(S(" files"));
		}
		print_char('\n');
		free(d.data[i].name.data);
	}

	if (d.data != nil)
		free(d.data);
}

void cmd_shutdown(str arg)
{
	(void) arg;

	// this only works in QEMU currently
	// TODO: use ACPI to make this portable
	outw(0x604, 0x2000);
}

void cmd_cheese(str arg)
{
	(void) arg;
	cheese_demo();
}

struct __attribute__((packed)) {
	u64 reg;
	u64 to;
	u64 from;
} watchdog_err;

void watchdog();

static void cmd_watchdog(str arg)
{
	(void) arg;

	str regs[] = {
		S("rax"), S("rbx"),
		S("rcx"), S("rdx"),
		S("rdi"), S("rsi"),
		S("rbp"), S("rsp"),
		S("r8"), S("r9"),
		S("r10"), S("r11"),
		S("r12"), S("r13"),
		S("r14"), S("r15"),
	};

	watchdog();

	print(S("watchdog: register "));
	print(regs[watchdog_err.reg]);
	print(S(" changed from "));
	print_hex(watchdog_err.from);
	print(S(" to "));
	print_hex(watchdog_err.to);
	print(S("\n"));
}

static void cmd_clocktest(str arg)
{
	(void) arg;
	while(1) {
		print_num(clock_monotonic(), 10);
		print(S(" "));
		print_num(clock_monotonic_coarse(), 10);
		//wait_irq();
		yield(nil);
		print(S("\r"));
	}
}

static void cmd_choose(str arg)
{
	str f = fs_read(arg);

	if (f.data == nil) {
		print(S("choose: file not found: "));
		print(arg);
		print(S("\n"));
	} else {
		str iter = f;
		int choices = 0;
		while (str_walk(&iter, S("\n")).len > 0)
			choices++;

		if (choices > 0) {
			iter = f;
			int chosen = rand() % choices;
			str line;
			for (int i = 0; i < chosen+1; i++)
				line = str_walk(&iter, S("\n"));
			print(line);
			print_char('\n');
		}

		free(f.data);
	}
}

typedef struct {
	str name;
	void (*fn)(str arg);
} command;

static command registry[] = {
	{ S("echo"),      &cmd_echo      },
	{ S("cat"),       &cmd_cat       },
	{ S("font"),      &cmd_font      },
	{ S("fontdemo"),  &cmd_fontdemo  },
	{ S("img"),       &cmd_img       },
	{ S("lspci"),     &cmd_lspci     },
	{ S("run"),       &cmd_run       },
	{ S("loadkeys"),  &cmd_loadkeys  },
	{ S("clear"),     &cmd_clear     },
	{ S("love"),      &cmd_love      },
	{ S("uname"),     &cmd_uname     },
	{ S("ls"),        &cmd_ls        },
	{ S("shutdown"),  &cmd_shutdown  },
	{ S("cheese"),    &cmd_cheese    },
	{ S("watchdog"),  &cmd_watchdog  },
	{ S("clocktest"), &cmd_clocktest },
	{ S("choose"),    &cmd_choose    },
};

void shell_run_cmd(str cmd)
{
	str prog = str_walk(&cmd, S(" \t"));

	if (prog.len == 0)
		return;

	for (usize i = 0; i < LEN(registry); i++) {
		if (str_cmp(prog, registry[i].name) == 0) {
			registry[i].fn(cmd);
			return;
		}
	}

	print(S("shell: unknown command: "));
	print(prog);
	print(S("\n"));
}

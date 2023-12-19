#include "shell.h"
#include "font.h"
#include "heap.h"
#include "fs.h"
#include "gfx.h"
#include "string.h"
#include "pci.h"
#include "memory.h"

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
			str cmd = str_split_walk(&iter, S("\n"));
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
		arg = S("anna");

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

typedef struct {
	str name;
	void (*fn)(str arg);
} command;

static command registry[] = {
	{ S("echo"),     &cmd_echo     },
	{ S("cat"),      &cmd_cat      },
	{ S("font"),     &cmd_font     },
	{ S("fontdemo"), &cmd_fontdemo },
	{ S("img"),      &cmd_img      },
	{ S("lspci"),    &cmd_lspci    },
	{ S("run"),      &cmd_run      },
	{ S("loadkeys"), &cmd_loadkeys },
	{ S("clear"),    &cmd_clear    },
	{ S("love"),     &cmd_love     },
};

void shell_run_cmd(str cmd)
{
	str prog = str_split_walk(&cmd, S(" \t"));

	if (prog.len == 0)
		return;

	for (usize i = 0; i < sizeof registry / sizeof *registry; i++) {
		if (str_cmp(prog, registry[i].name) == 0) {
			registry[i].fn(cmd);
			return;
		}
	}

	print(S("shell: unknown command: "));
	print(prog);
	print(S("\n"));
}

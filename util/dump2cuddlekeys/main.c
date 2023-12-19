#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

struct {
	char c;
	const char *name;
} defs[] = {
	{ '\n',  "Return"       },
	{ ' ',   "space"        },
	{ '\b',  "Delete"       },
	{ '!',   "exclam"       },
	{ '&',   "ampersand"    },
	{ '%',   "percent"      },
	{ '$',   "dollar"       },
	{ '\'',  "apostrophe"   },
	{ '(',   "parenleft"    },
	{ ')',   "parenright"   },
	{ '*',   "asterisk"     },
	{ '+',   "plus"         },
	{ ',',   "comma"        },
	{ '-',   "minus"        },
	{ '.',   "period"       },
	{ '/',   "slash"        },
	{ '0',   "zero"         },
	{ '1',   "one"          },
	{ '2',   "two"          },
	{ '3',   "three"        },
	{ '4',   "four"         },
	{ '5',   "five"         },
	{ '6',   "six"          },
	{ '7',   "seven"        },
	{ '8',   "eight"        },
	{ '9',   "nine"         },
	{ ':',   "colon"        },
	{ ';',   "semicolon"    },
	{ '<',   "less"         },
	{ '=',   "equal"        },
	{ '>',   "greater"      },
	{ '?',   "question"     },
	{ '\n',  "linefeed"     },
	{ '\\',  "backslash"    },
	{ '\t',  "Tab"          },
	{ '@',   "at"           },
	{ '[',   "bracketleft"  },
	{ ']',   "bracketright" },
	{ '{',   "braceleft"    },
	{ '}',   "braceright"   },
	{ '#',   "numbersign"   },
	{ '^',   "asciicircum"  },
	{ '\0',  "nul"          },
	{ '\xe', "Shift"        },
	{ '_',   "underscore"   },
	{ '"',   "quotedbl"     },
	{ '~',   "asciitilde"   },
	{ '`',   "grave"        },
};

static char lookup(char *key)
{
	size_t len = strlen(key);
	if (len == 1)
		return key[0];
	else if (len == 2 && key[0] == '+')
		return key[1];
	else for (size_t i = 0; i < sizeof defs / sizeof *defs; i++)
		if (strcmp(defs[i].name, key) == 0)
			return defs[i].c;

	fprintf(stderr, "unhandled: %s\n", key);
	return '\0';
}

int main()
{
	char keymap[256] = { '\0' };

	char *line = NULL;
	size_t cap = 0;
	ssize_t len;
	while ((len = getline(&line, &cap, stdin)) != -1) {
		int code;
		char plain[len+1];
		char shift[len+1];

		int items = sscanf(line, "keycode %d = %s %s", &code, plain, shift);
		if (items < 2 || code >= 127)
			continue;

		char p = lookup(plain);
		char s = items > 2 ? lookup(shift) : '\0';

		if (p != '\0') {
			if (s == '\0')
				s = toupper(p);

			keymap[code*2+0] = p;
			keymap[code*2+1] = s;
		}
	}

	if (line != NULL)
		free(line);

	fwrite(keymap, 1, 256, stdout);
}

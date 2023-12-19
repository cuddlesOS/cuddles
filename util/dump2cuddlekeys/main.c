#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main()
{
	char keymap[256] = { '\0' };

	char *line = NULL;
	size_t cap = 0;
	ssize_t len;
	while ((len = getline(&line, &cap, stdin)) != -1) {
		int code;
		char key[len+1];

		if (sscanf(line, "keycode %d = %s", &code, key) != 2)
			continue;

		char c = '\0';

		struct {
			char c;
			const char *name;
		} defs[] = {
			{ '\n', "Return"      },
			{ ' ',  "space"       },
			{ '\b', "Delete"      },
			{ '!',  "exclam"      },
			{ '&',  "ampersand"   },
			{ '%',  "percent"     },
			{ '$',  "dollar"      },
			{ '\'', "apostrophe"  },
			{ '(',  "parenleft"   },
			{ ')',  "parenright"  },
			{ '*',  "asterisk"    },
			{ '+',  "plus"        },
			{ ',',  "comma"       },
			{ '-',  "minus"       },
			{ '.',  "period"      },
			{ '/',  "slash"       },
			{ '0',  "zero"        },
			{ '1',  "one"         },
			{ '2',  "two"         },
			{ '3',  "three"       },
			{ '4',  "four"        },
			{ '5',  "five"        },
			{ '6',  "six"         },
			{ '7',  "seven"       },
			{ '8',  "eight"       },
			{ '9',  "nine"        },
			{ ':',  "colon"       },
			{ ';',  "semicolon"   },
			{ '<',  "less"        },
			{ '=',  "equal"       },
			{ '>',  "greater"     },
			{ '?',  "question"    },
			{ '\n', "linefeed"    },
			{ '\\', "backslash"   },
			{ '\t', "Tab"         },
			{ '@', "at"           },
			{ '[', "bracketleft"  },
			{ ']', "bracketright" },
			{ '(', "{"            },
			{ ')', "}"            },
			{ '#', "numbersign"   },
			{ '^', "asciicircum"  },
		};

		if (strlen(key) == 1)
			c = key[0];
		else if (strlen(key) == 2 && key[0] == '+')
			c = key[1];
		else for (size_t i = 0; i < sizeof defs / sizeof *defs; i++)
			if (strcmp(defs[i].name, key) == 0) {
				c = defs[i].c;
				break;
			}

		if (c != '\0')
			keymap[(unsigned char) code] = (char) c;
		else if (strcmp(key, "nul") != 0)
			{} // fprintf(stderr, "unhandled: %s\n", key);
	}

	if (line != NULL)
		free(line);

	fwrite(keymap, 1, 256, stdout);
}

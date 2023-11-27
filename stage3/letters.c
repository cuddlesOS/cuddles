#include "letters.h"

Letter letters[256];

void letters_init()
{
	letters[' '] = (Letter) {{
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0
	}};
	letters['!'] = (Letter) {{
		0, 1, 0,
		0, 1, 0,
		0, 1, 0,
		0, 0, 0,
		0, 1, 0
	}};
	letters['\"'] = (Letter) {{
		1, 0, 1,
		1, 0, 1,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0
	}};
	letters['#'] = (Letter) {{
		1, 0, 1,
		1, 1, 1,
		1, 0, 1,
		1, 1, 1,
		1, 0, 1
	}};
	letters['$'] = (Letter) {{
		0, 1, 0,
		1, 1, 1,
		0, 1, 0,
		1, 1, 1,
		0, 1, 0
	}};
	letters['%'] = (Letter) {{
		1, 0, 1,
		0, 0, 1,
		0, 1, 0,
		1, 0, 0,
		1, 0, 1
	}};
	letters['&'] = (Letter) {{
		1, 1, 0,
		1, 0, 0,
		1, 1, 1,
		1, 0, 1,
		1, 1, 1
	}};
	letters['\''] = (Letter) {{
		0, 1, 0,
		0, 1, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0
	}};
	letters['('] = (Letter) {{
		0, 1, 0,
		1, 0, 0,
		1, 0, 0,
		1, 0, 0,
		0, 1, 0
	}};
	letters[')'] = (Letter) {{
		0, 1, 0,
		0, 0, 1,
		0, 0, 1,
		0, 0, 1,
		0, 1, 0
	}};
	letters['*'] = (Letter) {{
		1, 0, 1,
		0, 1, 0,
		1, 0, 1,
		0, 0, 0,
		0, 0, 0
	}};
	letters['+'] = (Letter) {{
		0, 0, 0,
		0, 1, 0,
		1, 1, 1,
		0, 1, 0,
		0, 0, 0
	}};
	letters[','] = (Letter) {{
		0, 0, 0,
		0, 0, 0,
		0, 1, 0,
		0, 1, 0,
		1, 0, 0
	}};
	letters['-'] = (Letter) {{
		0, 0, 0,
		0, 0, 0,
		1, 1, 1,
		0, 0, 0,
		0, 0, 0
	}};
	letters['.'] = (Letter) {{
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 1, 0
	}};
	letters['/'] = (Letter) {{
		0, 0, 1,
		0, 0, 1,
		0, 1, 0,
		1, 0, 0,
		1, 0, 0
	}};
	letters['0'] = (Letter) {{
		0, 1, 0,
		1, 0, 1,
		1, 0, 1,
		1, 0, 1,
		0, 1, 0
	}};
	letters['1'] = (Letter) {{
		0, 0, 1,
		0, 1, 1,
		0, 0, 1,
		0, 0, 1,
		0, 0, 1
	}};
	letters['2'] = (Letter) {{
		1, 1, 1,
		1, 0, 1,
		0, 1, 0,
		1, 0, 0,
		1, 1, 1
	}};
	letters['3'] = (Letter) {{
		1, 1, 1,
		0, 0, 1,
		0, 1, 1,
		0, 0, 1,
		1, 1, 1
	}};
	letters['4'] = (Letter) {{
		1, 0, 1,
		1, 0, 1,
		1, 1, 1,
		0, 0, 1,
		0, 0, 1
	}};
	letters['5'] = (Letter) {{
		1, 1, 1,
		1, 0, 0,
		1, 1, 1,
		0, 0, 1,
		1, 1, 0
	}};
	letters['6'] = (Letter) {{
		1, 1, 1,
		1, 0, 0,
		1, 1, 1,
		1, 0, 1,
		1, 1, 1
	}};
	letters['7'] = (Letter) {{
		1, 1, 1,
		0, 0, 1,
		0, 1, 0,
		1, 0, 0,
		1, 0, 0
	}};
	letters['8'] = (Letter) {{
		1, 1, 1,
		1, 0, 1,
		1, 1, 1,
		1, 0, 1,
		1, 1, 1
	}};
	letters['9'] = (Letter) {{
		1, 1, 1,
		1, 0, 1,
		1, 1, 1,
		0, 0, 1,
		1, 1, 1
	}};
	letters[':'] = (Letter) {{
		0, 0, 0,
		0, 1, 0,
		0, 0, 0,
		0, 1, 0,
		0, 0, 0
	}};
	letters[';'] = (Letter) {{
		0, 1, 0,
		0, 0, 0,
		0, 1, 0,
		0, 1, 0,
		1, 0, 0
	}};
	letters['<'] = (Letter) {{
		0, 0, 0,
		0, 1, 0,
		1, 0, 0,
		0, 1, 0,
		0, 0, 0
	}};
	letters['='] = (Letter) {{
		0, 0, 0,
		1, 1, 1,
		0, 0, 0,
		1, 1, 1,
		0, 0, 0
	}};
	letters['>'] = (Letter) {{
		0, 0, 0,
		0, 1, 0,
		0, 0, 1,
		0, 1, 0,
		0, 0, 0
	}};
	letters['?'] = (Letter) {{
		1, 1, 1,
		0, 0, 1,
		0, 1, 0,
		0, 0, 0,
		0, 1, 0
	}};
	letters['@'] = (Letter) {{
		0, 0, 0,
		1, 1, 1,
		1, 0, 1,
		1, 1, 1,
		1, 1, 1
	}};
	letters['['] = (Letter) {{
		1, 1, 0,
		1, 0, 0,
		1, 0, 0,
		1, 0, 0,
		1, 1, 0
	}};
	letters[']'] = (Letter) {{
		0, 1, 1,
		0, 0, 1,
		0, 0, 1,
		0, 0, 1,
		0, 1, 1
	}};
	letters['\\'] = (Letter) {{
		1, 0, 0,
		1, 0, 0,
		0, 1, 0,
		0, 0, 1,
		0, 0, 1
	}};
	letters['^'] = (Letter) {{
		0, 1, 0,
		1, 0, 1,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0
	}};
	letters['_'] = (Letter) {{
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		1, 1, 1
	}};
	letters['`'] = (Letter) {{
		0, 1, 0,
		0, 0, 1,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0
	}};
	letters['~'] = (Letter) {{
		0, 0, 0,
		0, 0, 1,
		1, 1, 1,
		1, 0, 0,
		0, 0, 0
	}};
	letters['{'] = (Letter) {{
		0, 1, 1,
		0, 1, 0,
		1, 1, 0,
		0, 1, 0,
		0, 1, 1
	}};
	letters['}'] = (Letter) {{
		1, 1, 0,
		0, 1, 0,
		0, 1, 1,
		0, 1, 0,
		1, 1, 0
	}};
	letters['|'] = (Letter) {{
		0, 1, 0,
		0, 1, 0,
		0, 1, 0,
		0, 1, 0,
		0, 1, 0
	}};
	letters['A'] = letters['a'] = (Letter) {{
		1, 1, 1,
		1, 0, 1,
		1, 1, 1,
		1, 0, 1,
		1, 0, 1
	}};
	letters['B'] = letters['b'] = (Letter) {{
		1, 1, 0,
		1, 0, 1,
		1, 1, 1,
		1, 0, 1,
		1, 1, 0
	}};
	letters['C'] = letters['c'] = (Letter) {{
		1, 1, 1,
		1, 0, 0,
		1, 0, 0,
		1, 0, 0,
		1, 1, 1
	}};
	letters['D'] = letters['d'] = (Letter) {{
		1, 1, 0,
		1, 0, 1,
		1, 0, 1,
		1, 0, 1,
		1, 1, 0
	}};
	letters['E'] = letters['e'] = (Letter) {{
		1, 1, 1,
		1, 0, 0,
		1, 1, 1,
		1, 0, 0,
		1, 1, 1
	}};
	letters['F'] = letters['f'] = (Letter) {{
		1, 1, 1,
		1, 0, 0,
		1, 1, 1,
		1, 0, 0,
		1, 0, 0
	}};
	letters['G'] = letters['g'] = (Letter) {{
		1, 1, 1,
		1, 0, 0,
		1, 0, 1,
		1, 0, 1,
		1, 1, 1
	}};
	letters['H'] = letters['h'] = (Letter) {{
		1, 0, 1,
		1, 0, 1,
		1, 1, 1,
		1, 0, 1,
		1, 0, 1
	}};
	letters['I'] = letters['i'] = (Letter) {{
		1, 1, 1,
		0, 1, 0,
		0, 1, 0,
		0, 1, 0,
		1, 1, 1
	}};
	letters['J'] = letters['j'] = (Letter) {{
		0, 0, 1,
		0, 0, 1,
		0, 0, 1,
		0, 0, 1,
		1, 1, 1
	}};
	letters['K'] = letters['k'] = (Letter) {{
		1, 0, 1,
		1, 1, 0,
		1, 0, 0,
		1, 1, 0,
		1, 0, 1,
	}};
	letters['L'] = letters['l'] = (Letter) {{
		1, 0, 0,
		1, 0, 0,
		1, 0, 0,
		1, 0, 0,
		1, 1, 1
	}};
	letters['M'] = letters['m'] = (Letter) {{
		1, 0, 1,
		1, 1, 1,
		1, 0, 1,
		1, 0, 1,
		1, 0, 1
	}};
	letters['N'] = letters['n'] = (Letter) {{
		1, 0, 1,
		1, 0, 1,
		1, 1, 1,
		1, 1, 1,
		1, 0, 1
	}};
	letters['O'] = letters['o'] = (Letter) {{
		1, 1, 1,
		1, 0, 1,
		1, 0, 1,
		1, 0, 1,
		1, 1, 1
	}};
	letters['P'] = letters['p'] = (Letter) {{
		1, 1, 1,
		1, 0, 1,
		1, 1, 1,
		1, 0, 0,
		1, 0, 0
	}};
	letters['Q'] = letters['q'] = (Letter) {{
		1, 1, 1,
		1, 0, 1,
		1, 0, 1,
		1, 1, 1,
		0, 0, 1
	}};
	letters['R'] = letters['r'] = (Letter) {{
		1, 1, 1,
		1, 0, 1,
		1, 1, 1,
		1, 1, 0,
		1, 0, 1
	}};
	letters['S'] = letters['s'] = (Letter) {{
		1, 1, 1,
		1, 0, 0,
		1, 1, 1,
		0, 0, 1,
		1, 1, 1
	}};
	letters['T'] = letters['t'] = (Letter) {{
		1, 1, 1,
		0, 1, 0,
		0, 1, 0,
		0, 1, 0,
		0, 1, 0
	}};
	letters['U'] = letters['u'] = (Letter) {{
		1, 0, 1,
		1, 0, 1,
		1, 0, 1,
		1, 0, 1,
		1, 1, 1
	}};
	letters['V'] = letters['v'] = (Letter) {{
		1, 0, 1,
		1, 0, 1,
		1, 0, 1,
		1, 0, 1,
		0, 1, 0
	}};
	letters['W'] = letters['w'] = (Letter) {{
		1, 0, 1,
		1, 0, 1,
		1, 0, 1,
		1, 1, 1,
		1, 0, 1
	}};
	letters['X'] = letters['x'] = (Letter) {{
		1, 0, 1,
		1, 0, 1,
		0, 1, 0,
		1, 0, 1,
		1, 0, 1
	}};
	letters['Y'] = letters['y'] = (Letter) {{
		1, 0, 1,
		1, 0, 1,
		1, 1, 1,
		0, 1, 0,
		0, 1, 0
	}};
	letters['Z'] = letters['z'] = (Letter) {{
		1, 1, 1,
		0, 0, 1,
		0, 1, 0,
		1, 0, 0,
		1, 1, 1
	}};
}

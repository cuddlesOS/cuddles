#include <sys_user.h>

display (*display_current)();
display (*display_create)();
void (*display_add)(display parent, display child, i32 index);
void (*display_remove)(display child);
void (*display_pixel)(display d, pos p, color c);
void (*display_channel_add)(char *name, channel ch);
channel (*display_channel_get)(char *name);
void (*rc_grab)(void *rc);
void (*rc_drop)(void *rc);
void *(*rc_create)(u64 siz, void *delete);
void *(*weak_downgrade)(void *rc);
void *(*weak_upgrade)(void *weak)
void (*weak_drop)(void *weak);
channel (*channel_create)();

void link()
{
	void **calltable = 0x500;

	display_current = calltable[0];
	display_create = calltable[1];
	display_add = calltable[2];
	display_remove = calltable[3];
	display_pixel = calltable[4];
	display_channel_add = calltable[5];
	display_channel_get = calltable[6];
	rc_grab = calltable[7];
	rc_drop = calltable[8];
	rc_create = calltable[9];
	weak_downgrade = calltable[10];
	weak_upgrade = calltable[11];
	weak_drop = calltable[12];
	channel_create = calltable[13];
}

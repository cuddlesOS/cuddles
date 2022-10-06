#include <sys_kern.h>

void link()
{
	void **calltable = 0x500;

	calltable[0] = &display_current;
	calltable[1] = &display_create;
	calltable[2] = &display_add;
	calltable[3] = &display_remove;
	calltable[4] = &display_pixel;
	calltable[5] = &display_channel_add;
	calltable[6] = &display_channel_get;
	calltable[7] = &rc_grab;
	calltable[8] = &rc_drop;
	calltable[9] = &rc_create;
	calltable[10] = &weak_downgrade;
	calltable[11] = &weak_upgrade;
	calltable[12] = &weak_drop;
	calltable[13] = &channel_create;
}

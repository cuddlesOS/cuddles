#define STACK_SIZE 65536
#define CURRENT *((process *) 0x500)

process process_current()
{
	return CURRENT;
}

static void process_delete(process p)
{
	free(p->stack);
	free(p->code);
	map_clear(&p->channels);
}

static void process_push(process p, u64 value)
{
	p->stack_ptr -= sizeof value;
	*p->stack_ptr = value;
}

void process_exit()
{
	if (CURRENT->next == CURRENT)
		panic("init exited");

	CURRENT->prev->next = CURRENT->next;
	CURRENT->next->prev = CURRENT->prev;
	rc_drop(*current);

	process_yield();
}

process process_create(char *path, display d)
{
	process p = rc_create(sizeof *p, &process_delete);

	p->state = PROCESS_CREATED;

	p->stack = malloc(STACK_SIZE);
	p->stack_ptr = p->stack + STACK_SIZE;
	p->code = p->code_ptr = file_read(path);

	process_push(p, (u64) &process_exit);
	// push 6 callee resaved registers - contents dont actually matter.
	for (int i = 0; i < 6; i++)
		process_push(p, 0);

	p->displ = d;
	p->next = p->prev = nil;
	map_init(&p->channels);

	channel sig = channel_create();
	process_channel_add(p, "signal", sig);
	rc_drop(sig);

	channel stat = channel_create();
	process_channel_add(p, "status", stat);
	rc_drop(stat);

	return p;
}

void process_start(process p)
{
	if (p->code_ptr == nil)
		panic("attempt to start invalid process");

	if (p->state != PROCESS_CREATED)
		panic("attempt to start running or dead process");

	rc_grab(p);

	p->next = CURRENT->next;
	CURRENT->next = p;
	p->prev = CURRENT;
}

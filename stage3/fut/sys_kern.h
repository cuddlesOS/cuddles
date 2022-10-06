#ifndef _SYS_H_
#define _SYS_H_

// misc

typedef struct {
	i32 x, y;
} pos;

typedef struct {
	u32 width, height;
} bounds;

typedef struct {
	u8 r, g, b
} color;

// display

typedef struct display {
	pos p;
	bounds b;
	struct display *parent;
	u64 index;
} *display;

display display_current();
display display_create();
void display_add(display parent, display child, i32 index);
void display_remove(display child);

void display_pixel(display d, pos p, color c);

void display_channel_add(char *name, channel ch);
channel display_channel_get(char *name);

// rc

typedef struct __attribute__((packed)) {
	u32 ref;
	u32 own;
	void *data;
	void (*delete)(void *data);
} rc_header;

void rc_grab(void *rc);
void rc_drop(void *rc);

void *rc_create(u64 siz, void *delete);
void *weak_downgrade(void *rc);
void *weak_upgrade(void *weak)
void weak_drop(void *weak);

// channel

typedef struct {
	array listeners;
	array subscribers;
} *channel;

channel channel_create();

// buffered

typedef struct {
	void *active;
	array buffer;
} buffered;

// process

typedef struct process __attribute__((packed)) {
	void *code;
	void *stack;
	u64 *stack_ptr;
	struct process *next;
	struct process *prev;
	map channels;
	display displ;
	enum {
		PROCESS_CREATED,
		PROCESS_RUNNING,
		PROCESS_DEAD,
	} state;
} *process;

// setup pointers

void link();

#endif

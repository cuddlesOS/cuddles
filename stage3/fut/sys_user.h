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

extern display (*display_current)();
extern display (*display_create)();
extern void (*display_add)(display parent, display child, i32 index);
extern void (*display_remove)(display child);

extern void (*display_pixel)(display d, pos p, color c);

extern void (*display_channel_add)(char *name, channel ch);
extern channel (*display_channel_get)(char *name);

// rc

typedef struct __attribute__((packed)) {
	u32 ref;
	u32 own;
	void *data;
	void (*delete)(void *data);
} rc_header;

extern void (*rc_grab)(void *rc);
extern void (*rc_drop)(void *rc);

extern void *(*rc_create)(u64 siz, void *delete);
extern void *(*weak_downgrade)(void *rc);
extern void *(*weak_upgrade)(void *weak)
extern void (*weak_drop)(void *weak);

// channel

typedef struct {
	array listeners;
	array subscribers;
} *channel;

extern channel (*channel_create)();

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

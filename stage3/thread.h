#ifndef THREAD_H
#define THREAD_H

#include "def.h"

typedef struct {
	bool exit;
	i64 timeout;
} yield_arg;

void *yield(void *arg);

typedef struct {
	str name;
	void *stack_bottom;
	void *stack;
} thread;

typedef struct {
	u8 irq;
	union {
		u8 scancode;
	} data;
} event;

typedef struct {
	usize len;
	usize cap;
	event *data;
} event_queue;

extern event_queue queue_write;
extern thread *irq_services[16];

void thread_init();
void thread_resume(void *ret, thread *t);
thread *thread_create(str name, void *init);
void thread_sched(yield_arg *arg, void *stack);

#endif

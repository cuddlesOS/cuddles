#include "thread.h"
#include "heap.h"
#include "pic.h"

static thread *current_thread = nil;
void *thread_sched_stack = nil;

static event_queue queue_read = { 0, 0, nil };
event_queue queue_write = { 0, 0, nil };

thread *irq_services[16] = { nil };

void resume(void *stack, void *ret); // yield.asm

void thread_resume(void *ret, thread *t)
{
	current_thread = t;
	resume(t->stack, ret);
}

#define STACK_SIZE 0xffff

thread *thread_create(str name, void *init)
{
	thread *t = malloc(sizeof *t);
	t->name = name;
	t->stack_bottom = malloc(STACK_SIZE);
	t->stack = t->stack_bottom + STACK_SIZE - 8;
	*(void **) t->stack = init;
	t->stack -= 8*8;
	return t;
}

void thread_sched(yield_arg *arg, void *stack)
{
	if (current_thread != nil)
		current_thread->stack = stack;

	if (arg == nil) {
		// TODO: add to some sort of runqueue? (nil means not polling for anything)
	} else if (arg->exit) {
		free(current_thread->stack_bottom);
		free(current_thread);
		current_thread = nil;
	} else if (arg->timeout >= 0) {
		// TODO: meow
	}

	for (;;) {
		if (queue_read.len == 0) {
			disable_irqs();

			// swap queues
			event_queue tmp = queue_read;
			queue_read = queue_write;
			queue_write = tmp;

			enable_irqs();
		}

		if (queue_read.len > 0) {
			event *e = malloc(sizeof *e);
			*e = queue_read.data[--queue_read.len];

			if (irq_services[e->irq] == nil)
				free(e); // *shrug*
			else
				// this never returns. callee must free e
				thread_resume(e, irq_services[e->irq]);
		}

		wait_irq();
	}
}

void thread_init()
{
	thread_sched_stack = malloc(STACK_SIZE);

	queue_read = (event_queue) { 0, 1024, malloc(1024 * sizeof(event)) };
	queue_write = (event_queue) { 0, 1024, malloc(1024 * sizeof(event)) };
}

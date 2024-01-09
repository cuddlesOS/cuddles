#include "thread.h"
#include "heap.h"
#include "pic.h"
#include "clock.h"

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

#define STACK_SIZE 0x10000

// aligned on 16-byte boundary
static void *alloc_stack(void **alloc)
{
	usize stack = (usize) kmalloc(STACK_SIZE+16);
	if (alloc != nil)
		*alloc = (void *) stack;
	stack += 16 - stack % 16;
	return (void *) stack;
}

thread *thread_create(str name, void *init)
{
	thread *t = kmalloc(sizeof *t);
	t->name = name;
	t->stack = alloc_stack(&t->stack_bottom) + STACK_SIZE - 16;
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
		kfree(current_thread->stack_bottom);
		kfree(current_thread);
		current_thread = nil;
	} else if (arg->timeout >= 0) {
		// TODO: meow
	}

	for (;;) {
		clock_sync();
		if (queue_read.len == 0) {
			disable_irqs();

			// swap queues
			event_queue tmp = queue_read;
			queue_read = queue_write;
			queue_write = tmp;

			enable_irqs();
		}

		if (queue_read.len > 0) {
			event *e = kmalloc(sizeof *e);
			*e = queue_read.data[--queue_read.len];

			if (irq_services[e->irq] == nil)
				kfree(e); // *shrug*
			else
				// this never returns. callee must free e
				thread_resume(e, irq_services[e->irq]);
		}

		wait_irq();
	}
}

void thread_init()
{
	thread_sched_stack = alloc_stack(nil)+STACK_SIZE-8;

	queue_read = (event_queue) { 0, 1024, kmalloc(1024 * sizeof(event)) };
	queue_write = (event_queue) { 0, 1024, kmalloc(1024 * sizeof(event)) };
}

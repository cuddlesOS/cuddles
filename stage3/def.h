#ifndef DEF_H
#define DEF_H

#define nil ((void *) 0x0)

typedef unsigned char u8;
typedef signed char i8;

typedef unsigned short u16;
typedef short i16;

typedef unsigned int u32;
typedef int i32;

typedef unsigned long u64;
typedef long i64;

typedef u64 usize;
typedef i64 isize;

typedef u8 bool;
#define false ((bool) 0)
#define true ((bool) 1)

#define BITCAST(expr, from, to) (((union { from f; to t; }) { .f = expr }).t)

typedef struct {
	usize len;
	char *data;
} str;

#define S(x) ((str) { sizeof (x) - 1, (x) })
#define NILS ((str) { 0, nil })

#define BARRIER_VAR(var) asm volatile(""::"m"(var))
#define BARRIER() asm volatile("":::"memory")

#endif

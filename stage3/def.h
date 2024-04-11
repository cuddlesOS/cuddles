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
typedef i64 ssize; // TODO: replace by ssize_t

typedef u64 nanos;

typedef float f32;
typedef double f64;

typedef u8 bool;
#define false ((bool) 0)
#define true ((bool) 1)

#define BITCAST(expr, from, to) (((union { from f; to t; }) { .f = expr }).t)

#define BARRIER_VAR(var) asm volatile(""::"m"(var))
#define BARRIER() asm volatile("":::"memory")

#define LEN(x) (sizeof (x) / sizeof *(x))

#define MKVEC(T) typedef struct { usize len; T *data; } slice_ ## T; typedef struct { usize cap; slice_ ## T slice; } vec_ ## T;
#define NILS { .len = 0, .data = nil }
#define NILVEC { .cap = 0, .slice = NILS }

#define S(x) ((str) { sizeof (x) - 1, (x) })
#define SL(T, ...) ((slice_ ## T) { .len = LEN((T[]) { __VA_ARGS__ }), .data = (T[]) { __VA_ARGS__ } })

MKVEC(char)
MKVEC(bool)

MKVEC(u8) MKVEC(i8)
MKVEC(u16) MKVEC(i16)
MKVEC(u32) MKVEC(i32)
MKVEC(u64) MKVEC(i64)
MKVEC(usize) MKVEC(isize) MKVEC(ssize)
MKVEC(f32) MKVEC(f64)

typedef slice_char str;

typedef enum {
	ORD_LESS = -1,
	ORD_EQ = 0,
	ORD_GREATER = 1,
} ord;

#define ORD(a, b) ( \
	(a) < (b) ? ORD_LESS : \
	(a) > (b) ? ORD_GREATER : \
	ORD_EQ)

#define CHAIN_ORD(A, B) (((A) == ORD_EQ) ? (B) : (A))

#endif

#ifndef _DEF_H_
#define _DEF_H_

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

#endif

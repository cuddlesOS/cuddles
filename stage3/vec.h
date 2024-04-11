#ifndef T
#error "missing #define T"
#endif

#include "def.h"
#include "heap.h"
#include "memory.h"

#ifndef VEC_UTIL
#define VEC_UTIL

#define VECFN __attribute__((unused))

#define ITER_VAR(S, I) for (usize I = 0; I < S.len; I++)
#define ITER(S) ITER_VAR(S, i)

#define VEC_CAT(X, Y) X ## Y
#define VEC_CAT2(X, Y) VEC_CAT(X, Y)

static inline usize bit_ceil(usize n)
{
	return n <= 1 ? n : 1UL << (64 - __builtin_clzl(n-1));
}

#define VEC_LSEARCH(self, i, f, user_data, found, other) \
	for (usize i = 0; i < (self)->len; i++) { \
		if ((f)(&(self)->data[i], user_data)) { \
			found \
		} \
	} \
	other

#define VEC_BSEARCH(self, low, high, idx, f, user_data, found, other) \
	usize low, high, idx; \
	low = 0; \
	high = (self)->len; \
	while (low < high) { \
		idx = (low + high) / 2; \
		switch ((f)(&(self)->data[i], user_data)) { \
			case ORD_EQ: \
				found \
				break; \
			case ORD_LESS: high = idx; break; \
			case ORD_GREATER: low = idx + 1; break; \
		} \
	} \
	idx = low; \
	other

#endif

#define VEC VEC_CAT2(vec_, T)
#define SLICE VEC_CAT2(slice_, T)
#define FN(F) VEC_CAT2(VEC, _ ## F)

#ifdef BUILTIN_CMP
#define CMP(A, B) ORD(*(A), *(B))
#endif

#if defined(BUILTIN_EQ) || defined(BUILTIN_CMP)
#define EQ(A, B) (*(A) == *(B))
#endif

#if defined(CMP) && !defined(EQ)
#define EQ(A, B) (CMP(A, B) == ORD_EQ)
#endif

#undef BUILTIN_CMP
#undef BUILTIN_EQ

VECFN void FN(grow)(VEC *self, usize cap)
{
	if (self->cap < cap)
		self->cap = cap;
	else if (self->slice.data != nil)
		return;

	self->slice.data = krealloc(self->slice.data, sizeof(T) * self->cap);
}

VECFN void FN(insert)(VEC *self, usize pos, T data)
{
	heap_check();
	FN(grow)(self, bit_ceil(++self->slice.len));
	heap_check();
	rmemcpy(&self->slice.data[pos+1], &self->slice.data[pos], sizeof(T) * (self->slice.len-pos-1));
	heap_check();
	self->slice.data[pos] = data;
	heap_check();
}

VECFN T FN(remove)(SLICE *self, usize pos)
{
	heap_check();

	T x = self->data[pos];
	lmemcpy(&self->data[pos], &self->data[pos+1], sizeof(T) * (--self->len - pos));

	heap_check();
	return x;
}

VECFN void FN(append)(VEC *self, T data)
{
	FN(insert)(self, self->slice.len, data);
}

VECFN SLICE FN(clone_slice)(SLICE *self)
{
	SLICE other = NILS;
	other.data = kmalloc(sizeof(T) * (other.len = self->len));
	lmemcpy(other.data, self->data, sizeof(T) * self->len);
	return other;
}

VECFN VEC FN(clone)(VEC *self)
{
	return (VEC) {
		.slice = FN(clone_slice)(&self->slice),
		.cap = self->slice.len,
	};
}

VECFN bool FN(eqf)(SLICE *a, SLICE *b, bool f(T *a, T *b))
{
	if (a->len != b->len)
		return false;

	for (usize i = 0; i < a->len; i++) {
		if (!f(&a->data[i], &b->data[i]))
			return false;
	}

	return true;
}

VECFN ord FN(cmpf)(SLICE *a, SLICE *b, ord f(T *a, T *b))
{
	for (usize i = 0; i < a->len; i++) {
		if (i >= b->len)
			return ORD_GREATER;

		ord o = f(&a->data[i], &b->data[i]);
		if (o != ORD_EQ)
			return o;
	}

	return ORD_EQ;
}

VECFN bool FN(lsearchf)(SLICE *self, usize *pos, bool f(T *x, void *user_data), void *user_data)
{
	VEC_LSEARCH(self, i, f, user_data, *pos = i; return true;, return false;)
}

VECFN void FN(lset_addf)(VEC *self, bool f(T *a, T *b), T x)
{
	VEC_LSEARCH(&self->slice, i, f, &x,,FN(append)(self, x);)
}

VECFN void FN(lset_removef)(SLICE *self, bool f(T *a, T *b), T *x)
{
	VEC_LSEARCH(self, i, f, x, FN(remove)(self, i);,)
}

VECFN bool FN(bsearchf)(SLICE *self, usize *pos, ord f(T *x, void *user_data), void *user_data)
{
	VEC_BSEARCH(self, low, high, i, f, user_data, *pos = i; return true;, *pos = i; return false;)
}

VECFN void FN(bset_addf)(VEC *self, ord f(T *a, T *b), T x)
{
	VEC_BSEARCH(&self->slice, low, high, i, f, &x, return;, FN(insert)(self, i, x);)
}

VECFN void FN(bset_removef)(SLICE *self, ord f(T *a, T *b), T *x)
{
	VEC_BSEARCH(self, low, high, i, f, x, FN(remove)(self, i); return;,)
}

#ifdef EQ

VECFN bool FN(_eq_scalar)(T *a, T *b)
{
	return EQ(a, b);
}

VECFN bool FN(_eq_scalar_void)(T *a, void *b)
{
	return EQ(a, (T *) b);
}

VECFN bool FN(eq)(SLICE *a, SLICE *b)
{
	return FN(eqf)(a, b, FN(_eq_scalar));
}

VECFN bool FN(lsearch)(SLICE *self, usize *pos, T *x)
{
	return FN(lsearchf)(self, pos, FN(_eq_scalar_void), x);
}

VECFN void FN(lset_add)(VEC *self, T x)
{
	return FN(lset_addf)(self, FN(_eq_scalar), x);
}

VECFN void FN(lset_remove)(SLICE *self, T *x)
{
	return FN(lset_removef)(self, FN(_eq_scalar), x);
}

#endif

#ifdef CMP

VECFN ord FN(_cmp_scalar)(T *a, T *b)
{
	return CMP(a, b);
}

VECFN ord FN(_cmp_scalar_void)(T *a, void *b)
{
	return CMP(a, (T *) b);
}

VECFN ord FN(cmp)(SLICE *a, SLICE *b)
{
	return FN(cmpf)(a, b, FN(_cmp_scalar));
}

VECFN bool FN(bsearch)(SLICE *self, usize *pos, T *x)
{
	return FN(bsearchf)(self, pos, FN(_cmp_scalar_void), x);
}

VECFN FN(binsert)(VEC *self, T x)
{
	FN(lsearch)(&self->slice)
}

#endif

#undef T
#undef VEC
#undef FN
#undef CMP
#undef EQ

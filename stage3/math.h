#ifndef MATH_H
#define MATH_H

// for now, stuff will be added in this file as it is needed

#define PI 3.14159265358979323846

/*double get_pi()
{
	double pi;
	asm("fldpi; fstpl %0":"=m"(pi));
	return pi;
}*/

static inline double sin(double x)
{
	asm("fldl %1; fsin; fstpl %0":"=m"(x):"m"(x));
	return x;
}

static inline double rad(double x)
{
	return PI / 180.0 * x;
}

static inline double deg(double x)
{
	return 180.0 / PI * x;
}

static inline double fabs(double x)
{
	asm("fldl %1; fabs; fstpl %0":"=m"(x):"m"(x));
	return x;
}

static inline double atan2(double x, double y)
{
	asm("fldl %1; fldl %2; fpatan; fstpl %0":"=m"(x):"m"(x),"m"(y));
	return x;
}

static inline double sqrt(double x)
{
	asm("fldl %0; fsqrt; fstpl %0":"=m"(x):"m"(x));
	return x;
}

static inline double acos(double x)
{
	return atan2(sqrt(1-x*x),x);
}

static inline long ipow(long b, unsigned long exp)
{
	long x = 1;
	for (unsigned long i = 0; i < exp; i++)
		x *= b;
	return x;
}

#endif

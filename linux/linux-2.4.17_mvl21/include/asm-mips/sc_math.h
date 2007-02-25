/* Based upon asm-i386/sc_math.h for high resolution timers */
/* Copyright 2002, Telogy Networks, Inc. */
#ifndef SC_MATH
#define SC_MATH
#include "div64.h"

#define MATH_STR(X) #X
#define MATH_NAME(X) X

/*
 * Pre scaling defines
 */
#define SC_32(x) ((long long)x<<32)
#define SC_n(n,x) (((long long)x)<<n)
/*
 * This routine preforms the following calculation:
 *
 * X = (a*b)>>32
 * we could, (but don't) also get the part shifted out.
 */
extern inline unsigned long mpy_ex32(long a,long b)
{
	unsigned long long temp;

	temp = a;
	temp *= b;
	temp = temp >> 32;
	return (unsigned long) temp;
}
/*
 * X = (a/b)<<32 or more precisely x = (a<<32)/b
 */

extern inline long div_ex32(long a, long b)
{
	unsigned long res, high,low;
	high = a;
	low = 0;
	do_div64_32(res,high,low,b);
	return(res);
}
#define div_long_long_rem(a,b,c) div_ll_X_l_rem(a,b,c)
extern inline long div_ll_X_l_rem(long long divs, long div, long *rem)
{
	unsigned long a, b;
	unsigned long res;

	a = divs >> 32;
	b = divs & 0xFFFFFFFF;
	*rem = do_div64_32(res, a, b, div);
	return(res);
}

extern inline long div_ll_X_l(long long divs, long div)
{
	long dum;
	return div_ll_X_l_rem(divs,div,&dum);
}

/*
 * X = (a*b)>>24
 * we could, (but don't) also get the part shifted out.
 */

#define mpy_ex24(a,b) mpy_sc_n(24,a,b)
/*
 * X = (a/b)<<24 or more precisely x = (a<<24)/b
 */
#define div_ex24(a,b) div_sc_n(24,a,b)

/*
 * The routines allow you to do x = (a/b) << N and
 * x=(a*b)>>N for values of N from 1 to 32.
 *
 * These are handy to have to do scaled math.
 * Scaled math has two nice features:
 * A.) A great deal more precision can be maintained by
 *     keeping more signifigant bits.
 * B.) Often an in line div can be repaced with a mpy
 *     which is a LOT faster.
 *
 */
#define mpy_sc_n(N,aa,bb) \
({\
	unsigned long long a,b,c;\
	a = aa; b = bb;\
	c = (a * b);\
	c = c >> N;\
	c;\
})	

extern inline long div_sc_n(unsigned long N, unsigned long long divs, long div)
{
	register unsigned long res, high,low;
	high = divs >> (32- N);
	low = (divs << N) - high;
	do_div64_32(res,high,low,div);
	return(res);
}	
#endif

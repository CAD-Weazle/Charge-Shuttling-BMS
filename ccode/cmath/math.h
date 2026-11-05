/*
 *				m a t h . h
 */

#ifdef DOCUMENTATION
title	math.h	cmath constants and definitions
index	cmath constants and definitions
usage
	.s
	#include <math.h>
	.s
description
	.s
 	<math.h> is a file of constants and definitions used by the
	cmath library routines. As such it retains the commonly occurring
	magic numbers from that package in one convenient location.
	If any of those definitions have to be modified the cmath
	functions will need to be recompiled also. In some cases the
	accuracy of these constants is critical to the accuracy of
	some component of the cmath package, so it would be wise not
	to touch them unless you understand the significance for that
	package of your actions.
	.br
	It will usually not be necessary to include this file in a
	program which uses the cmath functions. However, since it
	contains useful constants like PI, E, PI_BY_2, ROOT_2 and so
	on it may be included to provide those.  Full details of its
	contents appear in the 'internal' section of this documentation.
	.s
diagnostics
	.s
	Not applicable
	.s
internal
	.s
	<math.h> contains 4 types of constants :-
	.s
	1) Absolute constants such as pi and e.
	.s
	2) Checked octal constants; these are absolute constants for
	which it has been verified that the compiler converts the decimal
	representation given here to a fully accurate binary number
	inside the machine. This level of accuracy is essential for at
	least one of the algorithms in the package.
	.s
	3) System dependent constants, which includes such things as the
	largest and smallest numbers, number of bits in the exponent and
	mantissa and so on.
	.s
	4) The symbolic values of the error codes for the cmathe error
	package.
	.s
	The symbolic names, values and descriptions of each of those
	groups of constants appears below.
	.s
	/*
	 * absolute constants
	 */
	.s
	PI 3.14159265358979324		pi
	.br
	HALF_PI 1.57079632679489662	pi/2
	.br
	REC_PI 0.318309886183790672	1/pi
	.br
	RPIBY2 0.636619772367581343	reciprocal of pi/2
	.br
	E 2.718281828459045235		e
	.br
	LOGBE2 0.69314718055994530942	log of 2 to base e
	.br
	LOGB2E 1.44269504088896341	log of e to base 2
	.br
	ROOT_2 1.4142135623730950488	square root of 2
	.br
	ROOT_3  1.732050807568877293	square root of 3
	.s
	/*
	 * checked octal constants
	 */
	.s
	PI_BY_2 1.57079632679489662	pi/2
	.br
	PI_BY_4 0.785398163397448310	pi/4
	.br
	LOGB10E 0.434294481903251828	log of e to base 10
	.br
	ROOT_05 0.70710678118654752440	square root of 0.5
	.s
	/*
	 * system dependent constants
	 */
	.s
	TINY 0.293873587705571877E-38	smallest no = 2**-128
	.br
	HUGE 0.170141183460469230E+39	largest no = 2**+127
	.br
	LOG_HUGE 0.880296919311130543E+02	log of HUGE
	.br
	LOG_TINY -0.887228391116729997E+02	log of TINY
	.br
	MIN_EXP -128			minimum base 2 exponent
	.br
	MAX_EXP 127			maximum base 2 exponent
	.br
	MAXLONG 017777777777L		largest long integer
	.br
	SIGFIGS 18			max no useful digits for dtoa
	.br
	TRIG_MAX 3.1415926535897932385e12	trig fn limit
	.br
	ROOT_EPS 0.372529029846191406e-8	2**-28
	.br
	REC_HUGE 0.587747175411143754E-38	1 / HUGE
	.s
	/*
	 * error codes to communicate with cmathe
	 */
	.s
	FP_OPER 1	/* FPU op code error			*/
	.br
	FP_ZDIV 2	/* FPU divide by zero			*/
	.br
	FP_FTOI 3	/* FPU float to integer conv error	*/
	.br
	FP_OFLO 4	/* FPU overflow				*/
	.br
	FP_UFLO 5	/* FPU underflow			*/
	.br
	FP_UDEF 6	/* FPU undefined variable (-0)		*/
	.br
	FP_BIGI 7	/* Atof input too large			*/
	.br
	FP_BADC 8	/* Bad character in atof input string	*/
	.br
	FP_NESQ 9	/* Square root of negative number	*/
	.br
	FP_LEXP	10	/* Exp argument too large		*/
	.br
	FP_SEXP 11	/* Exp argument too small		*/
	.br
	FP_NLOG 12	/* Log argument zero or negative	*/
	.br
	FP_TANE 13	/* Argument of tan too large		*/
	.br
	FP_TRIG 14	/* Argument of sin/cos too large	*/
	.br
	FP_ATAN 15	/* Atan2 arguments both zero		*/
	.br
	FP_COTE 16	/* Argument of cotan too small		*/
	.br
	FP_ARSC 17	/* Bad argument for asin/acos		*/
	.br
	FP_SINH 18	/* Argument of sinh too large		*/
	.br
	FP_COSH 19	/* Argument of cosh too large		*/
	.br
	FP_POWN 20	/* Negative argument in pow		*/
	.br
	FP_POWO 21	/* Result of pow overflows		*/
	.br
	FP_POWU 22	/* Result of pow underflows		*/
	.s
	/* numbers of each type of error - to determine argument type	*/
	.s
	FP_NFPU 6	/* No of FPU generated errors		*/
	.br
	FP_NSTR 2	/* No of string argument errors		*/
	.br
	FP_NMAR 14	/* No of math routine double arg errors	*/
	.s
author
	.s
	Hamish Ross
	.s
date
	.s
	24-Feb-85
#endif

/*
 * absolute constants
 */

#define PI 3.14159265358979324		/* pi				*/
#define HALF_PI 1.57079632679489662	/* pi/2				*/
#define REC_PI 0.318309886183790672	/* 1/pi				*/
#define RPIBY2 0.636619772367581343	/* reciprocal of pi/2		*/
#define E 2.718281828459045235		/* e				*/
#define LOGBE2 0.69314718055994530942	/* log of 2 to base e		*/
#define LOGB2E 1.44269504088896341	/* log of e to base 2		*/
#define ROOT_2 1.4142135623730950488	/* square root of 2		*/
#define ROOT_3  1.732050807568877293	/* square root of 3		*/

/*
 * checked octal constants
 */

#define PI_BY_2 1.57079632679489662	/* pi/2				*/
#define PI_BY_4 0.785398163397448310	/* pi/4				*/
#define LOGB10E 0.434294481903251828	/* log of e to base 10		*/
#define ROOT_05 0.70710678118654752440	/* square root of 0.5		*/

/*
 * system dependent constants
 */

#define TINY 0.293873587705571877E-38	/* smallest no = 2**-128	*/
#define HUGE 0.170141183460469230E+39	/* largest no = 2**+127		*/
#define LOG_HUGE 0.880296919311130543E+02	/* log of HUGE		*/
#define LOG_TINY -0.887228391116729997E+02	/* log of TINY		*/
#define MIN_EXP -128			/* minimum base 2 exponent	*/
#define MAX_EXP 127			/* maximum base 2 exponent	*/
#define MAXLONG 017777777777L		/* largest long integer		*/
#define SIGFIGS 18			/* max no useful digits in dtoa	*/
#define TRIG_MAX 3.1415926535897932385e12/* arg limit for trig functions*/
#define ROOT_EPS 0.372529029846191406e-8	/* 2**-(t/2), t = 56	*/
#define REC_HUGE 0.587747175411143754E-38/* 2**-127 = 1 / HUGE		*/

/*
 * error codes to communicate with cmathe
 */

#define FP_OPER 1		/* FPU op code error			*/
#define FP_ZDIV 2		/* FPU divide by zero			*/
#define FP_FTOI 3		/* FPU float to integer conv error	*/
#define FP_OFLO 4		/* FPU overflow				*/
#define FP_UFLO 5		/* FPU underflow			*/
#define FP_UDEF 6		/* FPU undefined variable (-0)		*/
#define FP_BIGI 7		/* Atof input too large			*/
#define FP_BADC 8		/* Bad character in atof input string	*/
#define FP_NESQ 9		/* Square root of negative number	*/
#define FP_LEXP	10		/* Exp argument too large		*/
#define FP_SEXP 11		/* Exp argument too small		*/
#define FP_NLOG 12		/* Log argument zero or negative	*/
#define FP_TANE 13		/* Argument of tan too large		*/
#define FP_TRIG 14		/* Argument of sin/cos too large	*/
#define FP_ATAN 15		/* Atan2 arguments both zero		*/
#define FP_COTE 16		/* Argument of cotan too small		*/
#define FP_ARSC 17		/* Bad argument for asin/acos		*/
#define FP_SINH 18		/* Argument of sinh too large		*/
#define FP_COSH 19		/* Argument of cosh too large		*/
#define FP_POWN 20		/* Negative argument in pow		*/
#define FP_POWO 21		/* Result of pow overflows		*/
#define FP_POWU 22		/* Result of pow underflows		*/

/* numbers of each type of error - used to determine argument type	*/

#define FP_NFPU 6		/* No of FPU generated errors		*/
#define FP_NSTR 2		/* No of string argument errors		*/
#define FP_NMAR 14		/* No of math routine double arg errors	*/

/* The following define error codes which are assigned to $$ferr by 
 * the cmathe error package. They will be flagged as unknown errors by
 * the perror() function and these error numbers printed.  Normally that
 * error reporting mechanism will not be used, it has been included for
 * benefit of programs which have been transported from systems which 
 * use the perror() function.						*/

#define SIGFPE 108	/* floating point exception error		*/
#define EDOM 133	/* domain error (input argument inadmissable)	*/
#define ERANGE 134	/* range error (result too large or small)	*/


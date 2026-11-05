/*
 *				s q r t . c
 */

#ifdef	DOCUMENTATION
title	sqrt	square root function
index	square root function
usage
	.s
	double x, f, sqrt();
	.br
	f = sqrt(x);
	.s
description
	.s
	Returns square root of argument.
	.s
diagnostics
	.s
	If the argument is negative the message 'sqrt arg negative', followed
	by the value of the argument, is written to stderr.
	The square root of the absolute value of the argument is returned.
	.s
internal
	.s
	Algorithm from pp. 17-34 of Cody and Waite. Uses 3 Newton iterations.
author
	.s
	Hamish Ross.
	.s
date
	.s
	26-Dec-84
#endif

#include <math.h>
double sqrt(x)
double x;
{
    double f, frexp(), ldexp(), y, z;
    int n;

    if (x == 0.0)
	return(x);
    f = frexp(x, &n);
    y = 0.4173102246 + 0.5901604053 * f;
    z = y + f / y;
    y = 0.25 * z + f / z;
    z = 0.5 * (y + f / y);
    if (n & 1 == 1) {
	n++;
	z *= 0.70710678118654752440;
    }
    return(ldexp(z, n / 2));
}

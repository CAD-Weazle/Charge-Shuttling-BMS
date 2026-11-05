/*
 *				l d e x p . c
 */

#ifdef	DOCUMENTATION
title	ldexp	load mantissa and exponent of a number to base 2
index	load mantissa and exponent of a number to base 2
usage
	.s
	double x, mant, ldexp();
	.br
	int xi;
	.br
	x = ldexp(mant, xi);
	.s
description
	.s
	This is the UNIX function ldexp. It returns the floating point
	number built from the exponent xi and mantissa mant.
	.s
diagnostics
	.s
	None
	.s
internal
	.s
	This does not exist as a C program. The program is written in
	macro, as ldexp.mac, since AS does not seem to handle the LDEXP
	instruction properly.
	This file is included only for the documentation.
	.s
author
	.s
	Hamish Ross.
	.s
date
	.s
	23-Aug-84
#endif

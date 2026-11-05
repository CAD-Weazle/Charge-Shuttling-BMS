// file    : printf.h
// author  : ao/rb
// purpose : header file for printf.c
// date    : 000101
// last    : 180914
//

#ifndef _PRINTF_H_
#define _PRINTF_H_

int printf (const char *fmt, ...);
int sprintf (char *buf, const char *fmt, ...);

int debug  (uint8_t do_print, const char *fmt, ...);

int putchar (int c);
int puts    (const char *s);

#endif





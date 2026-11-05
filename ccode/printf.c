// file    : printf.c
// author  : ao
// purpose : print formatted output to UART/string
// date    : 000101
// last    : 180914
//
// note: removing including 'stdio.h' in some STM32 header file might be needed

#define FLOAT

#include "includes.h"
#include <stdarg.h>
#include <limits.h>
#include <string.h>

#ifdef FLOAT
//#include "./cmath/dtoa.h"
#endif

// -- static prototypes
static int parsenum (const char **end);
static int vsprintf (char *s, const char *fmt, va_list args);
//static int sprintf  (char *buf, const char *fmt, ...);

enum
{
   ZEROPAD = 0x01
};

// parse string into number
static int parsenum (const char **end)
{
  const char *s = *end;
  int c;
  int val = 0;

  while ((c = *s - '0') >= 0 && c <= 9)
  {
    val = val * 10 + c;
    s++;
  }

  *end = s;

  return (val);
}

/*
 *
 * floating point format:
 *
 *   31  30  29  28  27  26  25  24  23  22  21  20       0 
 * +---+---+---+---+---+---+---+---+---+---+---+---+...+---+
 * | S |           exponent            |    fraction       |
 * +---+---+---+---+---+---+---+---+---+---+---+---+...+---+
 * 
 * exponent                 value
 * --------------------------------------------------
 * 0                        0.fraction * 2^-126
 * 1..254                   1.fraction * 2^(exp - 127)
 * 255                      infinity
 *
 */

union ieee
{
    float f;
    uint32_t u;
    struct
    {
        uint32_t frac: 23;
        uint32_t exp : 8;
        uint32_t sign: 1;
    };
};

static const uint32_t powten[8] = 
{
    10,
    100,
    1000,
    10000,
    100000,
    1000000,
    10000000,
    100000000,
};

char *fp_format( float x, int prec )
{
    static char buf[16];
    char *ptr = buf + sizeof(buf);
    union ieee v = { x };
    uint32_t i;                         // integer part
    uint32_t f = v.frac | 0x800000;     // fractional part
    uint64_t f64;
    int exp = v.exp - 127 - 23;         // corrected exponent

    if( v.exp == 255 )
    {
        if( v.frac ) return "NaN";
        if( v.sign ) return "-Inf";
        return "+Inf";
    }

    // determine integer part
    if( exp < -31 )         i = 0;
    else if( exp < 0 )      i = f >> -exp;
    else                    i = f << exp;

    // determine fractional part
    if( (exp += 32) < 0 )   f >>= -exp;
    else if( exp >= 32 )    f = 0;
    else                    f <<= exp;

    // make sure precision is reasonable
    if( prec < 0 ) prec = 0;
    if( prec > 7 ) prec = 7;

    f64 = f * (uint64_t) powten[prec];
    f = (f64 >> 32) + 5;
    if( (int) f64 < 0 )
        f++;
    f /= 10;

    // format backwards, starting with fractional part
    *--ptr = 0;
    if( prec > 0 )
    {
        while( --prec >= 0 ) 
        {
            *--ptr = '0' + f % 10;
            f /= 10;
        }
        *--ptr = '.';
    }

    // format integer part
    i += f;
    do
    {
        *--ptr = '0' + i % 10;
        i /= 10;
    } while( i );

    // add sign
    if( v.sign )
        *--ptr = '-';
    return ptr;
}

// sends argument list as formatted output to string
static int vsprintf (char *s, const char *fmt, va_list args)
{
  char c;
  const char *prefix, *p = s;
  char *arg;
  char buf[32];
  int flags, precision, len, width, zeropad, max;
  unsigned long val, base;

#ifdef FLOAT 
  double fval;  // rb
#endif

  while ((c = *fmt++) != '\0')
  {
    if (c != '%')
    {
      *s++ =  c;
      continue;
    }

    flags = zeropad = width = 0;
    precision = -1;
    max = INT_MAX;
    prefix = "";

    while (1)
    {
      c = *fmt;

      if (c == '0')
        flags = ZEROPAD;
      else
        break;

      fmt++;
    }

    width = parsenum (&fmt);

    if (*fmt == '.')
    {
      fmt++;
      precision = parsenum (&fmt);
    }

    if (*fmt == 'h' || *fmt == 'l')
      fmt++;

    switch ((c = *fmt++))
    {
      case 'u':
      case 'd':
        base = 10;
        break;

#ifdef FLOAT 
      case 'e':
      case 'f':
      case 'g':
        fval = va_arg (args, double);
        arg = fp_format( fval, precision );
        goto string;
#endif

      case 'p':
        precision = 2 * sizeof (void *);
      case 'X':
      case 'x':
        base = 16;
        break;

      case 'i':
        val = va_arg (args, unsigned long);
        sprintf (buf, "%d.%d.%d.%d",  
                 (unsigned char) val, 
                 (unsigned char) (val >> 8), 
                 (unsigned char) (val >> 16), 
                 (unsigned char) (val >> 24));
        arg = buf;
        goto string;

      case 's':
        arg = va_arg (args, char *);

        if (precision >= 0)
          max = precision;
          goto string;

      case 'c':
        buf[0] = (char) va_arg (args, int);
        arg = buf;
        max = 1;
        goto string;

      default:
        *s++ = c;
        continue;
    }

    //turn number into string, without padding
    val = (unsigned long) va_arg (args, int);

    if (c == 'd' && (long) val < 0)
    {
      val = -val;
      prefix = "-";
    }

    arg = buf + sizeof (buf);
    *--arg = 0;

    while (val != 0)
    {
      *--arg = "0123456789abcdef"[val % base];
      val /= base; 
    }

    if (precision >= 0)
      zeropad = precision;
    else if (flags & ZEROPAD)
      zeropad = width - strlen (prefix);
    else
      zeropad = 1;

string:
    len = strnlen (arg, max);

    if ((zeropad -= len) < 0)
      zeropad = 0;

    width -= len + zeropad + strlen (prefix);

    while (*prefix)
      *s++ = *prefix++;

    while (--zeropad >= 0)
      *s++ = '0';

    while (--len >= 0)
      *s++ = *arg++;

    while (--width >= 0)
      *s++ = ' ';
  }

  *s = 0;

  return (s - p);
}

// print to UART
int printf (const char *fmt, ...)
{
  va_list args;
  static char buf[120];

  va_start (args, fmt);
  vsprintf (buf, fmt, args);
  va_end (args);

  uart1_puts (buf);

  return strlen (buf);
}

// print to string
int sprintf (char *buf, const char *fmt, ...)
{
  va_list args;
  int n;

  va_start (args, fmt);
  n = vsprintf (buf, fmt, args);
  va_end (args);

  return (n);
}

// print debug to UART
int debug (uint8_t do_print, const char *fmt, ...)
{
  if (do_print)
  {
    va_list args;
    static char buf[120];

    va_start (args, fmt);
    vsprintf (buf, fmt, args);
    va_end (args);

    uart1_puts (buf);

    return strlen (buf);
  }
  else
    return -1;
}



// needed for replacement of define in 'stdio.h' 
int putchar (int c)
{
  uart1_putc (c);
  
  return 0;
}

// needed for replacement of define in 'stdio.h' 
int puts (const char *s)
{
  uart1_puts (s);
  uart1_putc ('\n');

  return 0;
}

// needed for replacement of define in 'stdio.h' 
int getchar (void)
{
  return uart1_getc ();
}




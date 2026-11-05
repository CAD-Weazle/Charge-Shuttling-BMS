/*
 *  dtoa.c
 */

#include <math.h>
//#include "dtoa.h"

#define PZERO 38		/* index of 1e0 in powten[]	*/
#define PMAX 76			/* highest index in powten[]	*/
#define SIGFIGS 18		/* number of significant digits in double */

extern const double powten[];

void ftoa( char *buff, char conv, int bsize, int dplace, double value  )
{
    double v;
    int i, imax, j, exp, ndigits, nlead;

    /* set default value of dplace */
    if( dplace < 0 )
	dplace = 6;

    /* strip off sign */
    if( value < 0.0 )
    {
	value = -value;
	*buff++ = '-';
    }

    /* scale and compute no of leading digits */
    if( value == 0.0 )
	imax = PZERO;
    else for( imax = PMAX; value < powten[imax] && imax > 0; imax--) 
    {
	if( conv == 'f' && imax <= PZERO )
	    break;
    }
    exp = imax - PZERO;
    nlead = exp + 1;

    /* decide if 'g' goes to 'e' or 'f' */
    if( conv == 'g') 
    {
	if( nlead > 6 )
	    conv = 'e';
	else
	    conv = 'f';
    }

    /* compute no of digits to print */
    /* change 'f' to 'e' if insufficient space for result */
    if( conv == 'f') 
    {
	ndigits = dplace + nlead;
	if( ndigits + 3 > bsize) 
	{
	    conv = 'e';
	    dplace = SIGFIGS;
	}
    }
    if( conv != 'f') 
    {
	nlead  = 1;
	ndigits = dplace + nlead;
	if( ndigits > SIGFIGS )
	    ndigits = SIGFIGS;
    }

    /* scale to range 1.0 - 10.0 and round up */
    if( conv == 'e' && imax == 0 && value < powten[0]) 
    {
	value *= 10.0;
	exp--;
    }
    value = value / powten[imax] + 0.5 * powten[PZERO - ndigits + 1];
    value = modf(value, &v);
    if( v >= 10.0) 
    {
	*buff++ = '1';
	v -= 10.0;
    }

    /* build digit string */
    for( i = 0; i < ndigits; i++) 
    {
	if( i == nlead )
	    *buff++ = '.';
	*buff++ = (int)v + '0';
	value = modf(value * 10.0, &v);
    }

    /* if not 'f' print exponent part */
    if( conv != 'f' )
    {
	*buff++ = 'e';
	if( exp < 0 )
	{
	    exp = -exp;
	    *buff++ = '-';
	}
	else *buff++ = '+';
	j = exp/10;
	*buff++ = '0' + j;
	*buff++ = '0' + exp - 10*j;
    }
    /* print final null */
    *buff++ = '\0';
    return;
}

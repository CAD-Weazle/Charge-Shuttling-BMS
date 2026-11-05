// file    : cli.c
// author  : rb
// purpose : command line interface 
// date    : 081104
// last    : 260911
//

#include "includes.h"
#include <ctype.h>
#include <strings.h>

#define INBUF_LEN RX1_SIZE

// globals
uint8_t buf_len;
char    buf[INBUF_LEN];

int   argc;
char *argv[10];

// print first prompt
void first_prompt (void)
{
  printf ("\n>");
}

// print prompt
void prompt (void)
{ 
  // normal prompt
  printf ("\r>");
}

// convert string to integer
int rb_atoi (char *s)
{
  int i = 0, n, sign = 1;
  
  if( s[0] == '-' )
  {
    sign = -1;
    i++;
  }
  for( n = 0; s[i] >= '0' && s[i] <= '9'; i++ )
    n = 10 * n + s[i] - '0';
  
  return (sign * n);
}

// convert string to long
long rb_atol (char *s)
{
  int i = 0, sign = 1;
  long n;
  
  if( s[0] == '-' )
  {
    sign = -1;
    i++;
  }
  
  for( n = 0; s[i] >= '0' && s[i] <= '9'; i++ )
    n = 10 * n + s[i] - '0';
  
  return( sign * n );
}

// skip white space
char *skipws (char *line)
{
  if( line != NULL )
  {
    while( *line == ' ' )
      line++;
  }
  return( line );
}

// break up line in words
int split_line (char *line, char **words)
{
  int n = 0;
  char *sep = line;
  
  while( (line = skipws(sep)) != NULL && *line )
  {
    sep = strchr( line, ' ' );
    if (sep)
      *sep++ = 0;
    words[n++] = line;
  }
  return( n );
}

// command parser
void parse_line (int argc, char **argv)
{
  if (argc)
  {
    // run BMS selftest
    if (!strcmp (argv[0], "pt"))
    {
    //bms_pass_run ();
    }

    // BMS on
    if (!strcmp (argv[0], "bms_ena"))
    {
      bms_enable ();
    }

    // BMS off
    if (!strcmp (argv[0], "bms_dus"))
    {
      bms_disable ();
    }

    // pass FET lower on 
    if (!strcmp (argv[0], "plo"))
    {
      uint16_t num = rb_atoi (argv[1]);

      cell_pass_lower (num);
    }

    // pass FET lower off
    if (!strcmp (argv[0], "blo"))
    {
      uint16_t num = rb_atoi (argv[1]);

      cell_block_lower (num);
    }

    // pass FET upper on 
    if (!strcmp (argv[0], "phi"))
    {
      uint16_t num = rb_atoi (argv[1]);

      cell_pass_upper (num);
    }

    // pass FET upper off
    if (!strcmp (argv[0], "bhi"))
    {
      uint16_t num = rb_atoi (argv[1]);

      cell_block_upper (num);
    }

    // set duty cycle, with smoothing to prevent abrupt duty cycle changes ("or just plain explosions")
    if (!strcmp (argv[0], "dc"))
    {
      uint16_t dat = rb_atoi (argv[1]);

      printf ("set duty-cyle to %d%%\n", dat);

      pwm_set (dat);
    }

    // disable DC/DC PWM signal
    if (!strcmp (argv[0], "pwm_off"))
    {
      printf ("PWM disabled\n");
      pwm_disable ();
    }

    // enable output FET
    if (!strcmp (argv[0], "out_ena") || !strcmp (argv[0], "oe"))
    {
      printf ("#output enabled\n");
      load_enable ();
	}

    // disable output FET
    if (!strcmp (argv[0], "out_dis") || !strcmp (argv[0], "od"))
    {
      printf ("#output disabled\n");
      load_disable ();
	}


    // enable charger input FET
    if (!strcmp (argv[0], "in_ena") || !strcmp (argv[0], "ie"))
    {
      printf ("#charger input enabled\n");
      charger_enable ();
	}

    // disable charger input FET
    if (!strcmp (argv[0], "in_dis") || !strcmp (argv[0], "id"))
    {
      printf ("#charger input disabled\n");
      charger_disable ();
	}

#ifdef LAT0R 
    // dump LCD image memory to serial as as TGA file
    if (!strcmp (argv[0], "prtscr"))
    {
      lcd_prtscr ();
    }
	
	// enable DC/DC loop control
    if (!strcmp (argv[0], "o"))
    {
      printf ("DC/DC loop control enabled\n");
      dcdc_on ();
    }

    // disable DC/DC loop control
    if (!strcmp (argv[0], "f"))
    {
      printf ("PWM loop control disabled\n");
      dcdc_off ();
    }

    // start logging to Teraterm
    if (!strcmp (argv[0], "log"))
      adc_log_on ();

    // stop logging to Teraterm
    if (!strcmp (argv[0], "q"))
      adc_log_off ();
#endif

    // dump help data
    if (!strcmp (argv[0], "help"))
      help ();

    // dump version information
    if (!strcmp (argv[0], "version"))
      version ();
  }   
} 

// dump help
void help (void)
{
  printf ("help              : this help\n\r");
  printf ("version           : version information\n\r");
}
     
// dump version info
void version (void)
{
  printf ("\n#- BMSTEST - Buck-Boost Charge Shuttling Battery Balancer - " VERSION " -\n");
}

// poll command line interface 
void poll_cli (void)
{
  // read next char
  int c = uart1_getc ();

  // test for Rx buffer empty
  if (c < 0)
    return;

  // add byte to buffer & parse
  if ((c == '\r') | (c == '\n'))       // check for enter to end string
  {
    printf ("\n");                     // print newline
  
    buf[buf_len] = '\0';               // /0 terminate buffer
    buf_len      = 0;                  // reset pointer
  
    argc = split_line (buf, argv);     // parse command
    parse_line (argc, argv);          
  
    prompt ();                         // display new prompt
  }
  else if (c == '\b' || c == 0x7f)     // check for backspace and delete
  {
    if (buf_len > 0)
      buf_len--;
  }
  else if (isprint(c) && buf_len < INBUF_LEN)
    buf[buf_len++] = c;
}









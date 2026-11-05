// file    : cli.h
// author  : rb
// purpose : header file for cli.c
// date    : 081104
// last    : 200710


#ifndef _CLI_H_
#define _CLI_H_

//#include "types.h"
 
// -- globals
extern uint8_t do_log;
extern uint8_t pass_thru;
extern uint8_t do_send;

// -- prototypes
void parse_line (int argc, char **argv);
int  split_line (char *line, char **words);

char *skipws (char *line);

int  rb_atoi (char *s);
long rb_atol (char *s);

void poll_cli     (void);
void first_prompt (void);
void prompt       (void);

void version (void);
void help    (void);

#endif







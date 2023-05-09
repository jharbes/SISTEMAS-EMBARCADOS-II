// Program TONES_1.C
//
// Gnerates 500 and 440 Hz tone on portb3.  Illustrates the use of TMR3 in conjunction
// with CCP2.
//
//  PIC18C452
//
//  PORTB3 (term 17) ----------------------||-------- SPKR
//
// Set Congiguration for CCP2 assigned to RB3.  (CCP2MX = 0)
//
// copyright, Peter H. Anderson, Baltimore, MD, Jan, '02

#case
#device PIC18C452

#include <defs_18c.h>
#include <delay.h>
#include <ser_18c.h>

#define MAKE_LONG(h, l)  (((long) h) << 8) | (l)

unsigned long period_time;

void main(void)
{
	ser_init();		// for debugging

	// set up timer3
	t3rd16 = 0;
	t3ckps1 = 0;	t3ckps0 = 0;	// 1:1 prescale
	tmr3cs = 0;	// internal clock - 1 usec per tick
	tmr3on = 1;

	// set up CCP2
	latb3 = 0;
	trisb3 = 0;

	t3ccp2 = 0;		t3ccp1 = 1;	// assign timer3 to CCP2, timer1 to CCP1

	ccp2m3 = 1;  ccp2m2 = 0; ccp2m1 = 0;  ccp2m0 = 0;	// make an output one on match

	period_time = 1136;

	// enable interrupts
	peie = 1;
	ccp2ie = 1;
	gieh = 1;

	while(1)
	{
       printf(ser_char, ".");

	  lattb5 = 1;
	  trisb5 = 0;
  	  period_time = 1136;	// 440 Hz
      delay_ms(1000);

      latb5 = 0;
      period_time = 1000;	// 500 Hz
	  delay_ms(1000);
	}
}

#int_ccp2
compare2_int_handler(void)
{
   unsigned long x;
   ccp2m0 = !ccp2m0;	     // toggle for the opposite
   x = MAKE_LONG(CCPR2H, CCPR2L);
   x = x + period_time;
   CCPR2H = x >> 8;
   CCPR2L = x & 0xff;
}

#int_default
default_handler(void)
{
}

#include <delay.c>
#include <ser_18c.c>

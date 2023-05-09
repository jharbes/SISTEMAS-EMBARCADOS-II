// Program COUNTER.C
//
// Counts the number of events on T13CKI (term  ) over 1.0 seconds and
// displays on the terminal.
//
// Use TIMER3 to time for 10 ms * 100
// Use TIMER1 to count events
//
// copyright, Peter H. Anderson, Baltimore, MD, Jan, '02

#case
#device PIC18C452

#include <defs_18c.h>
#include <delay.h>
#include <ser_18c.h>

#define MAKE_LONG(h, l)  (((long) h) << 8) | (l)

unsigned long period_time;

byte num_10ms;

void main(void)
{
    long count;

	ser_init();		// for debugging

    printf(ser_char, "\r\n..................\r\n");

	// set up timer3
	t3rd16 = 0;
	t3ckps1 = 0;	t3ckps0 = 0;	// 1:1 prescale
	tmr3cs = 0;	// internal clock - 1 usec per tick

	TMR3H = 0x00; TMR3L = 0x00;

	// set up CCP2
	CCPR2H = (byte) (10000 >> 8);
	CCPR1L = (byte) 10000;

	t3ccp2 = 0;		t3ccp1 = 1;	// assign timer3 to CCP2, timer1 to CCP1

	ccp2m3 = 1;  ccp2m2 = 0; ccp2m1 = 1;  ccp2m0 = 1;	// special event - resets Timer 3

	// set up timer 1 as counter
	t1rd16 = 0;
	t1ckps1 = 0;	t1ckps0 = 0;	// 1:1 prescale
	t1oscen = 0;
	tmr1cs = 1;
    t1sync = 1; // do not sync

	TMR1H = 0x00;		// number of events
	TMR1L = 0x00;

    num_10ms = 100;

    // turn on timers
	tmr3on = 1;
    tmr1on = 1;

	// enable interrupts
	peie = 1;
	ccp2ie = 1;
	gieh = 1;

	while(num_10ms)	/* loop */
	{
	}

	while(gieh)
	{
		gieh = 0;
	}

	peie = 0;
	ccp2ie = 0;
	ccp2if = 0;

	count = MAKE_LONG(TMR1H, TMR1L);
	printf(ser_char, "%lu", count);

    while(1)	// loop to keep emulator from stalling
    {
    }
}


#int_ccp2
compare2_int_handler(void)
{
   --num_10ms;
}

#int_default
default_handler(void)
{
}

#include <delay.c>
#include <ser_18c.c>

// Program IRRem_1.C
//
// Illustrates input capture using Timer 1 in conjuction with CCP1.  Function
// input_capture() camps on RC2/CCP1 (term 17) and when a transition of the specified
// start state occurs, the times at which the specified number of transitions occur
// are saved in array a.
//
// This program captures a pulse train and displays the result.
//
//      IR Receiver --------------------------- RC2/CCP1 (term 17)
//
// copyright, Peter H. Anderson, Baltimore, Jan, '02

#case
#device PIC18C452

#include <defs_18c.h>
#include <delay.h>
#include <ser_18c.h>

#define FALSE 0
#define TRUE !0

#define MAKE_LONG(h, l)  (((long) h) << 8) | (l)

void input_capture(unsigned long *a, byte start_state, byte num_transitions);
void print_array(unsigned long *a, byte num_ele);

byte ccp1_int_occ;


void main(void)
{

   unsigned long t_times[18];
   ser_init();
   while(1)
   {
       input_capture(t_times, 0, 18);
	   print_array(t_times, 18);
   }
}

void input_capture(unsigned long *a, byte start_state, byte num_transitions)
{
    byte n;
	// set up timer1
	t1oscen = 0;
	tmr1cs = 0;	// internal 1 usec clock
	t1ckps1 = 0;  t1ckps0 = 0;
	tmr1on = 1;


	// set up ccp1 for input capture
	t3ccp2 = 0;  t3ccp1 = 0;	// timer 1 is clock source for both CCP modules
	ccp1m3 = 0; ccp1m2 = 1; ccp1m1 = 0;  ccp1m0 = start_state;

    // enable interrupts
    ccp1_int_occ = FALSE;
	peie = 1;
	ccp1ie = 1;
	gieh = 1;

	for (n=0; n<num_transitions; n++)
	{
		while(!ccp1_int_occ)	/* loop */  ;
		while(gieh)
		{
			gieh = 0;
		}

	    ccp1_int_occ = FALSE;
		a[n] = MAKE_LONG(CCPR1H, CCPR1L);

		gieh = 1;

	}

	while(gieh)
	{
		gieh = 0;
	}
	ccp1ie = 0;
	peie = 0;
}

void print_array(unsigned long *a, byte num_ele)
{
	byte n;

	printf(ser_char, "****************\r\n");

	for (n=0; n<num_ele; n++)
	{
		printf(ser_char, "%2x%2x\r\n", (byte) (a[n] >> 8), (byte) (a[n] & 0xff));
	}
}


#int_ccp1
ccp1_int_handler(void)
{
	// invert the m0 bit
	ccp1m0 = !ccp1m0;
	ccp1_int_occ = TRUE;
}

#int_default
default_int_handler(void)
{
}

#include <delay.c>
#include <ser_18c.c>

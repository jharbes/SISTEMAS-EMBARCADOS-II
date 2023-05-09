// Program STEP_1.C
//
// Turns stepper at 300 pulses per second (3333 usec per step) in direction indicated
// by switch on PORTB7.  Uses Timer 1 in conjunction with CCP1 in Compare - Trigger
// Special Event mode which resets the timer to 0 on a match of TMR1H & L and CCP1PR1H & L
//
//
// 	PIC18C452					ULN2803		Stepper
//
//  PORTB3 (term 36) --------- 1      18 ----------------- PHI3
//  PORTB2 (term 35) --------- 2      17 ----------------- PHI2
//  PORTB1 (term 34) --------- 3      16 ----------------- PHI1
//  PORTB0 (term 33) --------- 4      15 ----------------- PHI0
//
//                              GRD - Term 9
//                              Vdiode - Term 10
//
//						PIC18C452
//
// GRD ------ \------ PORTB7 (term 40)
//
//
// copyright, Peter H. Anderson, Baltimore, MD, Jan, '02


#case
#device PIC18C452

#include <defs_18c.h>
#include <delay.h>
#include <ser_18c.h>	// for possible debugging

#define FALSE 0
#define TRUE !0


const byte patts[8] = {0x01, 0x03, 0x02, 0x06, 0x04, 0x0c, 0x08, 0x09};
byte ccp1_int_occ, index;

void main(void)
{

	ccp1_int_occ = FALSE;
	index = 0;

    not_rbpu = 0;	// enable weak pullups
	LATB = 0x0f;	// lower nibble at logic one
	TRISB = 0xf0;	// lower nibble outputs


    // set up timer1
    t1rd16 = 1;
    t1ckps1 = 0;	t3ckps0 = 0;	// 1:1 prescale
    tmr1cs = 0;	// internal clock - 1 usec per tick
    TMR1H = 0x00; TMR1L = 0x00;


	// setup CCP1
	t3ccp2 = 0;		t3ccp1 = 1;	// assign timer3 to CCP2, timer1 to CCP1
    ccp1m3 = 1;  ccp1m2 = 0; ccp1m1 = 1;  ccp1m0 = 1;	// special event - resets Timer 1
    CCPR1H = (byte) (3333 >> 8);
    CCPR1L = (byte) 3333;

	// turn on timer
	tmr1on = 1;

	// enable ints
    ccp1if = 0;
    ccp1ie = 1;
    peie = 1;
    gieh = 1;


	while(1)
	{
	   if (ccp1_int_occ)
	   {
		   ccp1_int_occ = FALSE;
		   if (portb7)
		   {
		       ++index;	// note that this is global
		       if (index > 7)
		       {
		          index = 0;
		       }
		   }

		   else
		   {
		   	  --index;
		   	  if (index == 0xff)
		   	  {
		   		  index = 7;
 		      }
	      }
      }
   } // of while(1)
}


#int_ccp1
ccp1_int_handler(void)
{
	LATB = (LATB & 0xf0) | patts[index];
	ccp1_int_occ = TRUE;
}

#int_default
default_int_handler(void)
{
}

#include <delay.c>
#include <ser_18c.c>

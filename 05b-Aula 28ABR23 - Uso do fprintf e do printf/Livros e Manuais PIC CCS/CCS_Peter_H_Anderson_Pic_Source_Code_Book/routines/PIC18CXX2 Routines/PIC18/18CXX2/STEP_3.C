// Program STEP_3.C
//
// Uses a potentiometer on AN0 (term 2) to control the direction and speed of
// a stepper on the lower four bits of PORTB.
//
// Uses Timer 3 in conjuction with the CCP2 module configured to trigger a sepcial
// event (rest of timer plus perform an A/D conversion).  This is set to 10 ms such that
// an A/D conversion is performed every 10 ms
//
// Uses Timer 1 with CCP1 to trigger a special special event (reset timer).  This is used
// to control the delay between outputting half steps.
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
// copyright, Peter H. Anderson, Baltimore, MD, Jan, '02


#case
#device PIC18C452

#include <defs_18c.h>
#include <delay.h>
#include <ser_18c.h>

#define FALSE 0
#define TRUE !0

#define CW 0
#define CCW 1

#define MAKE_LONG(h, l)  (((long) h) << 8) | (l)

const byte patts[8] = {0x01, 0x03, 0x02, 0x06, 0x04, 0x0c, 0x08, 0x09};
byte ccp1_int_occ, ccp2_int_occ, index;

void main(void)
{
	byte dir;
	unsigned long x, step_delay_time, ad_sample_time;

	ccp1_int_occ = FALSE;
	ccp2_int_occ = FALSE;

	index = 0;
    step_delay_time = 20000;	// 20 ms
    dir = CW;
    ad_sample_time = 10000; // every 10 ms

	LATB = 0x0f;
	TRISB = 0xf0;

	// configure A/D
	pcfg3 = 0; pcfg2 = 1; pcfg1 = 0; pcfg0 = 0; 	// config A/D for 3/0

	adfm = 1;   // right justified
	adcs2 = 0; adcs1 = 1; adcs0 = 1; // internal RC

	adon=1;  // turn on the A/D
    chs2=0;  chs1=0;   chs0=0;

	// config timer 1
    t1rd16 = 1;
    t1ckps1 = 0;	t3ckps0 = 0;	// 1:1 prescale
    tmr1cs = 0;	// internal clock - 1 usec per tick
    TMR1H = 0x00; TMR1L = 0x00;

	// config timer 3
    t3rd16 = 1;
    t3ckps1 = 0;	t3ckps0 = 0;	// 1:1 prescale
    tmr3cs = 0;	// internal clock - 1 usec per tick
    TMR3H = 0x00; TMR3L = 0x00;

	// assign timers
    t3ccp2 = 0;		t3ccp1 = 1;	// assign timer3 to CCP2, timer1 to CCP1

    // configure CCP1
    ccp1m3 = 1;  ccp1m2 = 0; ccp1m1 = 1;  ccp1m0 = 1;	// special event - resets Timer 1
    CCPR1H = (byte) (step_delay_time >> 8);
    CCPR1L = (byte) (step_delay_time);


	// config ccp2
	ccp2m3 = 1;  ccp2m2 = 0; ccp2m1 = 1;  ccp2m0 = 1;	// special event - resets Timer 3 and initiates A/D
	CCPR2H = (byte) (ad_sample_time >> 8);
    CCPR2L = (byte) (ad_sample_time);

 	// turn on timers and config interrupts
    tmr1on = 1;
    tmr3on = 1;

    ccp1if = 0;
    ccp2if = 0;
    ccp1ie = 1;
    ccp2ie = 1;
    peie = 1;
    gieh = 1;

    while(1)
	{
	   if (ccp1_int_occ)
	   {
#asm
		   CLRWDT
#endasm
		   ccp1_int_occ = FALSE;
		   CCPR1H = (byte) (step_delay_time >> 8);
		   CCPR1L = (byte) step_delay_time;

		   // either increment or decrement
		   if (dir ==CW)
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

       if (ccp2_int_occ)
       {
#asm
		   CLRWDT
#endasm
		   ccp2_int_occ = FALSE;
		   x = MAKE_LONG(ADRESH, ADRESL);
		   if (x >= 0x0200)
		   {
			   x = x - 0x0200;  // in range of 0 to 0x01ff
			   step_delay_time = 4 * (0x0200 - x) + 1000;
			   dir = CW;
		   }

		   else
		   {
			   step_delay_time = 4 * x + 1000;
			   dir = CCW;
		   }

       }
   }	// end of while 1

}

#int_ccp1
ccp1_int_handler(void)
{
	LATB = (LATB & 0xf0) | patts[index];
	ccp1_int_occ = TRUE;
}

#int_ccp2
ccp2_int_handler(void)
{
	ccp2_int_occ = TRUE;
}

#int_default
default_int_handler(void)
{
}

#include <delay.c>
#include <ser_18c.c>

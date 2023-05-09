// Program STEP_4.C
//
// Accelerates, runs and decelerates a stepping motor over a period of time.
//
// Accelerates from 10000 us between half steps to 1500 us between half steps over
// a time interval of 25 ms.  Runs for 5000 ms with 1500 us between half steps.
// Decelerates to 10000 us over a time of 25 ms.
//
// This is repeated in the opposite direction.
//
// 	PIC18C452					ULN2803		Stepper
//
//  PORTB3 (term) ------------ 1      18 ----------------- PHI3
//  PORTB2 (term )------------ 2      17 ----------------- PHI2
//  PORTB1 (term )------------ 3      16 ----------------- PHI1
//  PORTB0 (term )------------ 4      15 ----------------- PHI0
//
//                              GRD - Term 9
//                              Vdiode - Term 10
//
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

void run(byte dir, unsigned long run_delay, unsigned long time_ms);
void accelerate(byte dir, unsigned long start_delay, unsigned long stop_delay, unsigned long time_ms);
void decelerate(byte dir, unsigned long start_delay, unsigned long stop_delay, unsigned long time_ms);

const byte patts[8] = {0x01, 0x03, 0x02, 0x06, 0x04, 0x0c, 0x08, 0x09};
byte ccp1_int_occ, ccp2_int_occ, index;

void main(void)
{
	ccp1_int_occ = FALSE;
	ccp2_int_occ = FALSE;

	index = 0;

	LATB = 0x0f;
	TRISB = 0xf0;

	while(1)
	{
	   accelerate(CW, 10000, 1500, 25);		// accelerate over 25 ms
	   run(CW, 1500, 5000);					// run for 5000 ms
	   decelerate(CW, 1500, 10000, 25);	// decelerate over 25 ms

	   accelerate(CCW, 10000, 1500, 250);
	   run(CCW, 1500, 5000);
	   decelerate(CCW, 1500, 10000, 25);

	   delay_ms(1000);
   }
}

void run(byte dir, unsigned long run_delay, unsigned long time_ms)
{

   // set up timer1
   t1rd16 = 0;
   t1ckps1 = 0;	t1ckps0 = 0;	// 1:1 prescale
   tmr1cs = 0;	// internal clock - 1 usec per tick
   TMR1H = 0x00; TMR1L = 0x00;

   // set up timer3
   t3rd16 = 0;
   t3ckps1 = 0;	t3ckps0 = 0;	// 1:1 prescale
   tmr3cs = 0;	// internal clock - 1 usec per tick
   TMR3H = 0x00; TMR3L = 0x00;

   // assign timers
   t3ccp2 = 0;		t3ccp1 = 1;	// assign timer3 to CCP2, timer1 to CCP1

   // setup CCP1
   ccp1m3 = 1;  ccp1m2 = 0; ccp1m1 = 1;  ccp1m0 = 1;	// special event - resets Timer 1 - used for step delay
   CCPR1H = (byte) (run_delay >> 8);
   CCPR1L = (byte) (run_delay);

   // setup CCP2
   ccp2m3 = 1;  ccp2m2 = 0; ccp2m1 = 1;  ccp2m0 = 1;	// special event - resets Timer 3 - one ms
   CCPR2H = (byte) (1000 >> 8);
   CCPR2L = (byte) 1000;

   // turn on timers
   tmr1on = 1;
   tmr3on = 1;

   ccp1if = 0;	// kill any pending interrupts
   ccp2if = 0;

   ccp1ie = 1;
   ccp2ie = 1;
   peie = 1;
   gieh = 1;

   ccp1_int_occ = FALSE;
   ccp2_int_occ = FALSE;

   while(time_ms)
   {

      if(ccp1_int_occ)	// step delay
      {
         ccp1_int_occ = FALSE;

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

	 if (ccp2_int_occ)	// time delay
	 {
		 ccp2_int_occ = FALSE;
		 --time_ms;
	 }

  }

  while(gieh)
  {
	  gieh = 0;
  }

  ccp1if = 0;	// kill any interrupt
  ccp2if = 0;

  tmr1on = 0;	// turn off timers
  tmr3on = 0;

  ccp1ie = 0;
  ccp2ie = 0;
}


void accelerate(byte dir, unsigned long start_delay, unsigned long stop_delay, unsigned long time_ms)
{
   unsigned long n, d, delay, new_match;
   float x, y;

   n = 0;

    // set up timer1
   t1rd16 = 1;
   t1ckps1 = 0;	t1ckps0 = 0;	// 1:1 prescale
   tmr1cs = 0;	// internal clock - 1 usec per tick
   TMR1H = 0x00; TMR1L = 0x00;

   // set up timer3
   t3rd16 = 1;
   t3ckps1 = 0;	t3ckps0 = 0;	// 1:1 prescale
   tmr3cs = 0;	// internal clock - 1 usec per tick
   TMR3H = 0x00; TMR3L = 0x00;

   // assign timers
   t3ccp2 = 0;		t3ccp1 = 1;	// assign timer3 to CCP2, timer1 to CCP1

   // setup CCP1
   ccp1m3 = 1;  ccp1m2 = 0; ccp1m1 = 1;  ccp1m0 = 0;	// software interrupt - used for step delay
   CCPR1H = (byte) (start_delay >> 8);
   CCPR1L = (byte) (start_delay);

   // setup CCP2
   ccp2m3 = 1;  ccp2m2 = 0; ccp2m1 = 1;  ccp2m0 = 1;	// trigger special event - reset timer 1
   CCPR2H = (byte) (1000 >> 8);
   CCPR2L = (byte) 1000;

   // turn on timers
   tmr1on = 1;
   tmr3on = 1;

   ccp1if = 0;	// kill any pending interrupts
   ccp2if = 0;

   ccp1ie = 1;
   ccp2ie = 1;
   peie = 1;
   gieh = 1;

   ccp1_int_occ = FALSE;
   ccp2_int_occ = FALSE;

   while(n<time_ms)
   {

      if(ccp1_int_occ)
      {
         ccp1_int_occ = FALSE;

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
		 ccp2_int_occ = FALSE;
		 ++n;
#ifdef FLOAT
		 // update CCPR1H & L
		 x = (float) (start_delay - stop_delay);
		 y = (float) (n) / (float) (time_ms);
		 x = x * y;

		 d = (unsigned long) x;
#else
        d = (start_delay - stop_delay)/128 * n / time_ms;
#endif
	     delay = start_delay - d;
	     new_match = MAKE_LONG(CCPR1H, CCPR1L) + delay;
	     CCPR1H = (byte) (new_match >> 8);
         CCPR1L = (byte) new_match;
	 }

   }

   while(gieh)
   {
	  gieh = 0;
   }

   ccp1if = 0;	// kill any interrupt
   ccp2if = 0;

   tmr1on = 0;	// turn off timers
   tmr3on = 0;

   ccp1ie = 0;
   ccp2ie = 0;
}


void decelerate(byte dir, unsigned long start_delay, unsigned long stop_delay, unsigned long time_ms)
{
   unsigned long n, d, delay, new_match;
   float x, y;

   n = 0;

    // set up timer1
   t1rd16 = 1;
   t1ckps1 = 0;	t1ckps0 = 0;	// 1:1 prescale
   tmr1cs = 0;	// internal clock - 1 usec per tick
   TMR1H = 0x00; TMR1L = 0x00;

   // set up timer3
   t3rd16 = 1;
   t3ckps1 = 0;	t3ckps0 = 0;	// 1:1 prescale
   tmr3cs = 0;	// internal clock - 1 usec per tick
   TMR3H = 0x00; TMR3L = 0x00;

   // assign timers
   t3ccp2 = 0;		t3ccp1 = 1;	// assign timer3 to CCP2, timer1 to CCP1

   // setup CCP1
   ccp1m3 = 1;  ccp1m2 = 0; ccp1m1 = 1;  ccp1m0 = 0;	// software interrupt - used for step delay
   CCPR1H = (byte) (start_delay >> 8);
   CCPR1L = (byte) (start_delay);

   // setup CCP2
   ccp2m3 = 1;  ccp2m2 = 0; ccp2m1 = 1;  ccp2m0 = 1;	// trigger special event - reset timer 1
   CCPR2H = (byte) (1000 >> 8);
   CCPR2L = (byte) 1000;

   // turn on timers
   tmr1on = 1;
   tmr3on = 1;

   ccp1if = 0;	// kill any pending interrupts
   ccp2if = 0;

   ccp1ie = 1;
   ccp2ie = 1;
   peie = 1;
   gieh = 1;

   ccp1_int_occ = FALSE;
   ccp2_int_occ = FALSE;

   while(n<time_ms)
   {

      if(ccp1_int_occ)
      {
         ccp1_int_occ = FALSE;

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
		 ccp2_int_occ = FALSE;
		 ++n;

#ifdef FLOAT
	     x = (float) (stop_delay - start_delay);
	     y = (float) (n) / (float) (time_ms);
	     x = x * y;

	     d = (unsigned long) x;
#else
        d = (start_delay - stop_delay)/128 * n / time_ms;
#endif

	     delay = start_delay + d;

	     new_match = MAKE_LONG(CCPR1H, CCPR1L) + delay;
	     CCPR1H = (byte) (new_match >> 8);
         CCPR1L = (byte) new_match;
	 }

   }
   while(gieh)
   {
	  gieh = 0;
   }

   ccp1if = 0;	// kill any interrupt
   ccp2if = 0;

   tmr1on = 0;	// turn off timers
   tmr3on = 0;

   ccp1ie = 0;
   ccp2ie = 0;

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

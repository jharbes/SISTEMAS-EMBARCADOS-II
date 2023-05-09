// Program STEP_2.C
//
// Illustrates accelerating, running and decelerating a stepper for a defined number of steps.
//
// From rest the stepper is linearly accelerated from 10000 us per half step to 1000 us per half
// step over 300 half steps of travel.  The motor is then run at 1000 usec per half step for
// 5000 half steps and then decelerated over 300 half steps.  This is repeated in the other direction.
//
// 	PIC18C452					ULN2803		Stepper
//
//  PORTB3 (term 36) --------- 1      18 ----------------- PHI3
//  PORTB2 (term 35) --------- 2      17 ----------------- PHI2
//  PORTB1 (term 34) --------- 3      16 ----------------- PHI1
//  PORTB0 (term 33) --------- 4      15 ----------------- PHI0
//
//
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

//#define FLOAT

void run(byte dir, unsigned long run_delay, unsigned long num_steps);
void accelerate(byte dir, unsigned long start_delay, unsigned long stop_delay, unsigned long num_steps);
void decelerate(byte dir, unsigned long start_delay, unsigned long stop_delay, unsigned long num_steps);

const byte patts[8] = {0x01, 0x03, 0x02, 0x06, 0x04, 0x0c, 0x08, 0x09};
byte ccp1_int_occ, index;

void main(void)
{
	ccp1_int_occ = FALSE;
	index = 0;

	LATB = 0x0f;
	TRISB = 0xf0;

	while(1)
	{
	   accelerate(CW, 10000, 1000, 300);
	   run(CW, 1000, 5000);
	   decelerate(CW, 1000, 10000, 300);

	   accelerate(CCW, 10000, 1000, 300);
	   run(CCW, 1000, 5000);
	   decelerate(CCW, 1000, 10000, 300);

	   delay_ms(1000);
   }
}

void run(byte dir, unsigned long run_delay, unsigned long num_steps)
{

   unsigned long n;

   // set up timer1
   t1rd16 = 0;
   t1ckps1 = 0;	t3ckps0 = 0;	// 1:1 prescale
   tmr1cs = 0;	// internal clock - 1 usec per tick
   TMR1H = 0x00; TMR1L = 0x00;

   // setup CCP1
   t3ccp2 = 0;		t3ccp1 = 1;	// assign timer3 to CCP2, timer1 to CCP1
   ccp1m3 = 1;  ccp1m2 = 0; ccp1m1 = 1;  ccp1m0 = 1;	// special event - resets Timer 1

   tmr1on = 1;
   ccp1if = 0;
   ccp1ie = 1;
   peie = 1;
   gieh = 1;

   CCPR1H = (byte) (run_delay >> 8);
   CCPR1L = (byte) run_delay;

   ccp1_int_occ = FALSE;

   for (n=0; n<num_steps; n++)
   {


      while(!ccp1_int_occ)
      {
         /* loop */
      }

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

  while(gieh)
  {
	  gieh = 0;
  }

  ccp1if = 0;	// kill any interrupt
  tmr1on = 0;
  ccp1ie = 0;
}


void accelerate(byte dir, unsigned long start_delay, unsigned long stop_delay, unsigned long num_steps)
{
   unsigned long d, n, delay;
   float x, y;

   // set up timer1
   t1rd16 = 0;
   t1ckps1 = 0;	t1ckps0 = 0;	// 1:1 prescale
   tmr1cs = 0;	// internal clock - 1 usec per tick
   TMR1H = 0x00; TMR1L = 0x00;

   // setup CCP1
   t3ccp2 = 0;		t3ccp1 = 1;	// assign timer3 to CCP2, timer1 to CCP1
   ccp1m3 = 1;  ccp1m2 = 0; ccp1m1 = 1;  ccp1m0 = 1;	// special event - resets Timer 1

   tmr1on = 1;
   ccp1if = 0;
   ccp1ie = 1;
   peie = 1;
   gieh = 1;

   ccp1_int_occ = FALSE;

   for (n=0; n<num_steps; n++)
   {
#ifdef FLOAT
	  x = (float) (start_delay - stop_delay);
	  y = (float) (n) / (float) (num_steps);
	  x = x * y;

	  d = (unsigned long) x;

	  delay = start_delay - d;
#else
      delay = start_delay - (start_delay - stop_delay)/128 * n / num_steps * 128;
#endif
      CCPR1H = (byte) (delay >> 8);
      CCPR1L = (byte) delay;


      while(!ccp1_int_occ)
      {
         /* loop */
      }

      ccp1_int_occ = FALSE;

      if (dir == CW)
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

  while(gieh)
  {
	  gieh = 0;
  }

  ccp1if = 0;	// kill any interrupt
  tmr1on = 0;
  ccp1ie = 0;
}

void decelerate(byte dir, unsigned long start_delay, unsigned long stop_delay, unsigned long num_steps)
{
   unsigned long d, n, delay;
   float x, y;

   // set up timer1
   t1rd16 = 0;
   t1ckps1 = 0;	t1ckps0 = 0;	// 1:1 prescale
   tmr1cs = 0;	// internal clock - 1 usec per tick
   TMR1H = 0x00; TMR1L = 0x00;

   // setup CCP1
   t3ccp2 = 0;		t3ccp1 = 1;	// assign timer3 to CCP2, timer1 to CCP1
   ccp1m3 = 1;  ccp1m2 = 0; ccp1m1 = 1;  ccp1m0 = 1;	// special event - resets Timer 3

   tmr1on = 1;
   ccp1if = 0;
   ccp1ie = 1;
   peie = 1;
   gieh = 1;

   ccp1_int_occ = FALSE;

   for (n=0; n<num_steps; n++)
   {
#ifdef FLOAT
	  x = (float) (stop_delay - start_delay);
	  y = (float) (n) / (float) (num_steps);
	  x = x * y;

	  d = (unsigned long) x;

	  delay = start_delay + d;
#else
      delay = start_delay + (stop_delay - start_delay)/128 * n / num_steps * 128;
#endif

      CCPR1H = (byte) (delay >> 8);
      CCPR1L = (byte) delay;

      while(!ccp1_int_occ)
      {
         /* loop */
      }

      ccp1_int_occ = FALSE;

      if (dir == CW)
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

  while(gieh)
  {
	  gieh = 0;
  }

  ccp1if = 0;	// kill any interrupt
  tmr1on = 0;
  ccp1ie = 0;
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

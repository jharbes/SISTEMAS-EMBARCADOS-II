// OUT_CMP1.C
//
// Illustrates Use of CCP1 for Output Compare.
//
// LED on RC2/CCP1 is continually turned off for four seconds and
// then on for 1.25 secs, etc. while processsor is continually send dots
// to LCD module.
//
// RC2/CCP1 (term 17) to LED.
//
// Note that a CCP1 interrupt in the Compare, Set on Match and Clr on Match
// does not reset TMR1.  Rather, the periodicity is achieved by adding an
// offset to CCPR1H and L.   This is important when using both CCP modules
// in the compare mode.
//
// Peter H. Anderson, Baltimore, MD, Jan, '01

#case

#device PIC16F877 *=16 ICD=TRUE

#include <defs_877.h>
#include <lcd_out.h>

#define TRUE !0
#define FALSE 0

void main(void)
{

   lcd_init();

   trisc2 = 0;		//CCP1 output to LED
   portc2 = 0;

   t1ckps1 = 1;		// set the prescaler for 1:8.  Thus, full roll is 16 secs
   t1ckps0 = 1;

   t1oscen = 1;		// turn on external 32.768 kHz osc
   tmr1cs = 1;

   t1sync = 0;		// synchronize external clock input.  Uncertain what this does for me.

   CCP1CON = 0x08;	// configure for output compare, set on match

   TMR1H = 0;		// start timer at 0x0000
   TMR1L = 0;

   CCPR1H = 0x00;
   CCPR1L = 20;		// first interrupt in 20 ticks - the value isn't critical.  But there is
   					// no point in waiting 16 secs.

   tmr1on = 1;		// turn on the timer

   ccp1if = 0;		// clear flag
   ccp1ie = 1;		// enable interrupts

   peie = 1;
   gie = 1;

   while(1)
   {
	  lcd_clr_line(0);	// can now be doing other things
      printf(lcd_char, "Hello World");
      delay_ms(1000);
   }
}

#int_ccp1 ccp1_int_handler(void)
{
   unsigned long next_time, current_time;

   if(CCP1CON==0x08)	// if it is currently set on match
   {
       next_time=0x1400;	// for 1.25 seconds
							// 1/32.768 kHz * 8 * X = 1.25; X=0x1400
   }

   else	// it is currently clear on match
   {
      next_time=0x4000;		// for 4.00 seconds
							// 1/32.768 kHz * 8 * X = 1.25; X=0x4000

   }
   // set new value of CCPR1H and L
   current_time = CCPR1H;
   current_time = current_time << 8 | CCPR1L;
   next_time = current_time + next_time;
   CCPR1H = next_time >> 8;
   CCPR1L = next_time;

   if(CCP1CON==0x08)	// if it is currently set on match
   {
      CCP1CON = 0x09;	// clear on match
   }
   else	// it is currently clear on match
   {
      CCP1CON=0x08;	// set on match
   }
}

#int_default default_int_handler(void)
{
}

#include <lcd_out.c>

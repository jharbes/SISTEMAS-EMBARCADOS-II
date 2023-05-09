// OUT_CMP2.C
//
// Illustrates Use of both CCP1 and CCP2 for Output Compare.
//
// LED on RC2/CCP1 is continually turned off for four seconds and
// then on for 1.25 secs, etc. while processsor is continually send dots
// to LCD module.
//
// RC2/CCP1 (term 17) to LED.
//
// Using CCP2 in Compare - Software Interrupt only.  Every 50 ms, scans
// RB0.  If at logic zero zero, keyval is set to 'A' and  displayed.
// Note that this is a cheap excuse for a keyboard scan routine.
//
// Peter H. Anderson, Baltimore, MD, Jan, '01

#case

#device PIC16F877 *=16 ICD=TRUE

#include <defs_877.h>
#include <lcd_out.h>

#define TRUE !0
#define FALSE 0

byte keypressed=FALSE;
byte keyval;

void main(void)
{
   byte pos = 0;
   lcd_init();

   not_rbpu = 0;	// enable weak pullups

   trisc2 = 0;		//CCP1 output to LED
   portc2 = 0;

   t1ckps1 = 1;		// set the prescaler for 1:8.  Thus, full roll is 16 secs
   t1ckps0 = 1;

   t1oscen = 1;		// turn on external 32.768 kHz osc
   tmr1cs = 1;

   t1sync = 0;		// synchronize external clock input.  Uncertain what this does for me.


   CCP1CON = 0x08;	// configure for output compare - set on match
   CCP2CON = 0x0a;	// output capture - software interrupt

   TMR1H = 0;
   TMR1L = 0;

   CCPR1H = 0x40;
   CCPR1L = 0x00;	// 4.00 secs

   CCPR2H = 0x00;
   CCPR2L = 208;	// about 50 ms

   tmr1on = 1;		// turn on the timer

   ccp1if = 0;		// clear flags
   ccp2if = 0;
   ccp1ie = 1;		// enable interrupts
   ccp2ie = 1;

   peie = 1;
   gie = 1;

   while(1)
   {
	  if (keypressed)
	  {
		  lcd_cursor_pos(3, pos);
		  lcd_char(keyval);
		  ++pos;
		  if (pos == 20)
		  {
			  pos = 0;
		  }
		  keypressed = FALSE;
      }
      else
      {
		  lcd_cursor_pos(0, 0);
          printf(lcd_char, "Hello World"); // can now be doing other things
      }
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

#int_ccp2 ccp2_int_handler(void)
{
   unsigned long next_time, current_time;
   next_time = 205;	// 1/32.768 kHz * 8 * 205 = 50.04 ms
   current_time = CCPR2H;
   current_time = current_time << 8 | CCPR2L;
   next_time = current_time + next_time;
   CCPR2H = next_time >> 8;
   CCPR2L = next_time;

   if(!rb0)	// this is a cheap excuse for a keyboard scan routine
   {
	  keypressed = TRUE;
	  keyval = 'A';
   }
}

#int_default default_int_handler(void)
{
}

#include <lcd_out.c>


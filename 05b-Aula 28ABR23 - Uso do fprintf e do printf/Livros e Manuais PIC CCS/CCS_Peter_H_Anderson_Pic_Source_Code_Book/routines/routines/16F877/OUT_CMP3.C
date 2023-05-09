// OUT_CMP3.C
//
// Illustrates Use of CCP2 for Output Compare - Trigger Special Event.
//
// Performs an A/D conversion on RA0/AN0 (Term 2) every 4.0 seconds.
//
// Peter H. Anderson, Baltimore, MD, Jan, '01

#case

#device PIC16F877 *=16 ICD=TRUE

#include <defs_877.h>
#include <lcd_out.h>

#define TRUE !0
#define FALSE 0

byte new_ad_avail = FALSE;
long ad_val;

void main(void)
{

   lcd_init();

   // configure A/D
   adfm = 1;   // a/d format right justified

   pcfg3 = 1; // configure for AN0 only
   pcfg2 = 1;
   pcfg1 = 1;
   pcfg0 = 0;

   adcs1 = 1;
   adcs0 = 1; // internal RC

   adon = 1;

   chs2 = 0; chs1 = 0; chs0 = 0;	// channel 0

   // set up Timer 1
   t1ckps1 = 1;		// set the prescaler for 1:8.  Thus, full roll is 16 secs
   t1ckps0 = 1;

   t1oscen = 1;		// turn on external 32.768 kHz osc
   tmr1cs = 1;

   t1sync = 0;		// synchronize external clock input.  Uncertain what this does for me.

   CCP2CON = 0x0b;	// output capture - trigger special event

   TMR1H = 0;
   TMR1L = 0;

   CCPR2H = 0x40;
   CCPR2L = 0x00;	// 4.00 secs

   tmr1on = 1;	// turn on the timer

   ccp2if = 0;	// kill any pending interrupts
   adif = 0;
   				// enable interrupts
   ccp2ie = 1;
   adie = 1;

   peie = 1;
   gie = 1;

   while(1)
   {
	  if (new_ad_avail)
	  {
		  lcd_clr_line(3);
		  printf(lcd_char, "%ld", ad_val);
		  new_ad_avail = FALSE;
      }
      else
      {
		  lcd_cursor_pos(0, 0);
          printf(lcd_char, "Hello World"); // can now be doing other things
      }
   }
}

#int_ccp2 ccp2_int_handler(void)
{
   unsigned long next_time, current_time;
   next_time = 0x4000;
   current_time = CCPR2H;
   current_time = current_time << 8 | CCPR2L;
   next_time = current_time + next_time;
   CCPR2H = next_time >> 8;
   CCPR2L = next_time;
}

#int_ad ad_int_handler(void)
{
   ad_val = ADRESH;				// fetch the value
   ad_val = ad_val << 8 | ADRESL;
   new_ad_avail = TRUE;			// signal that a/d int occurred
}

#int_default default_int_handler(void)
{
}

#include <lcd_out.c>




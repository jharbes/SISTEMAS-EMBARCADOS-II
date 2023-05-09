// COUNT_1.C
//
// Illustrates the use of Timer 0 as an evemt counter and Timer 1 for timing.
//
// Function count_1_sec configures Timer 0 as a counter of events appearing on
// PIC input TOCK1/RA4.
//
//
// copyright, Peter H. Anderson, Baltimore, Jan, '01


#case

#device PIC16F877 *=16 ICD=TRUE

#include <defs_877.h>
#include <lcd_out.h>

#define TRUE !0
#define FALSE 0

unsigned long count_1_sec(void);

int timer0_int_occurred;
int timer1_int_occurred;

void main(void)
{
   unsigned long count;
   float freq;

   lcd_init();
   while(1)
   {
      count = count_1_sec();

      lcd_clr_line(0);
      printf(lcd_char, "%ld", count);
      delay_ms(1000);
   }
}

unsigned count_1_sec(void)
{
   // uses tmr1 with 32 kHz to time for one second
   // use tmr0 to count the number of events

   byte high_byte = 0, low_byte;
   unsigned long count;

   timer0_int_occurred = FALSE;
   timer1_int_occurred = FALSE;

   t1ckps1=0;		// 1:1 prescale
   t1ckps0=0;

   t1oscen=1;		// enable external osc
   t1sync=1;		// don't synch external clock with CPU clock
   tmr1cs=1;		// external clock source

   delay_ms(5);		// a bit of time to let the oscillator turn on

   t0cs =1; 		// assign tmr0 counter to RA4
   psa = 1;			// prescaler not assigned to counter

   TMR0 = 0;		// init counts to zero

   TMR1H = 0x80;	// set TMR1 to roll over in 32,768 counts
   TMR1L = 0x00;

   tmr1on=1;		// turn on timer1
   t0se=1;			// enable counting from T0CK1 input

   t0if=0;
   t0ie=1;

   tmr1if = 0;		// clear any interrupt
   tmr1ie = 1;

   peie = 1;
   gie = 1;

   while(!timer1_int_occurred)
   {
      if(timer0_int_occurred)
      {
         timer0_int_occurred = FALSE;
         ++high_byte;	// there was a rollover of timer 0
      }
   }

   low_byte = TMR0;	// immediately fetch low 8 bits

   while(gie)
   {
      gie=0;
   }

   t0ie = 0;	// clean up
   t0se = 0;

   tmr1ie = 0;
   t1oscen = 0;
   tmr1on = 0;

   count = high_byte;
   count = (count << 8) | low_byte;
   return(count);
}

#int_rtcc timer_0_interrupt_handler()
{
   timer0_int_occurred = TRUE;
}

#int_timer1 timer_1_interrupt_handler()
{
   timer1_int_occurred = TRUE;
}

#int_default default_interrupt_handler()
{
}

#include <lcd_out.c>


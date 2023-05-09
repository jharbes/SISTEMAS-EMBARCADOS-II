// Program TMR0_1.C
//
// Illustrates use of TMR0 to time for 1 ms.  Generates 500 Hz tone on
// speaker on RD.0 and continually flashes LED on RD.4 on for 4 secs
// and off for 4 seconds.
//
// Note that TMR0 is configured for CLOCK, assigned to OSC, prescale
// by 4.  Thus, 1.00 MHz / 8 = 125 KHz.  Period = 8 usecs.  Thus, TMRO
// is loaded with the twos comp of 125 to achieve interrupt timing of 1 ms
//
// Copyright, Peter H. Anderson, Baltimore, MD, Jan, '01

#case

#device PIC16F877 *=16 ICD=TRUE

#include <defs_877.h>
#include <lcd_out.h>

#define TRUE !0
#define FALSE 0

#define T_TICK (~125) + 1	// see program description

void main(void)
{

   lcd_init();
   pspmode = 0;

   portd0 = 0;
   trisd0 = 0;		// speaker

   portd4 = 0;
   trisd4 = 0;		// LED


// configure TMR0
   t0cs = 0;		// use CLK as source
   psa = 0;			// prescaler assigned to TMR0
   ps2 = 0;  ps1 = 1;  ps0 = 0;	// prescale by 8


   t0if = 0; 		// clear any existing interrupt
   t0ie = 1;
   gie = 1;
   while(1)
   {
      lcd_clr_line(0);
      printf(lcd_char, "Hello World");
      delay_ms(500);

   }
}


#int_rtcc timer0_int_handler(void)
{
   static long isr_timer=4000;

   TMR0 = TMR0 + T_TICK;
   portd0 = !portd0;		// invert bit on speaker

   if (--isr_timer==0)
   {
      portd4 = !portd4;		// reverse LED every 4000 ms
      isr_timer=4000;
   }
}

#include <lcd_out.c>

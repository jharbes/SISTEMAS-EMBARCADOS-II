// Capture2.C
//
// Illustrates use of Timer 1 and CCP1 in the input capture mode to measure the
// logic zero and logic one times of a pulse appearing on CCP1/RC2.
//
// In function measure_pulse(), timer 1 is configured for internal osc, prescale of 1:1.
// Thus, one usec per tick.
//
// If the specified state is 0, the CCP mode is set to 0x04 so as to cause an interrupt
// on a falling edge.  If the specified state is 1, the mode is set to 0x05 to cause an
// interrupt on the rising edge.
//
// After the capture interrupt occurs, the CCP mode is set for either the rising or falling
// edge.
//
// After the second capture interrupt occurs, elapsed time from the first interrupt to the
// second is fetch from CCP1H & L and combined with the value of rollovers.
//
// Note that if, in waiting for either the first or second capture interrupt, the number of
// rollovers of timer 1 equals the specified maximum time to wait, success is set to FALSE.
//
// Copyright, Peter H. Anderson, Baltimore, MD, Jan, '01

#case

#device PIC16F877 *=16 ICD=TRUE

#include <defs_877.h>
#include <lcd_out.h>


#define FALSE 0
#define TRUE !0

float measure_pulse(byte state, byte max_rollovers, byte *p_success);

byte tmr1_int_occ, capture_int_occ;


void main(void)
{
   float t_pulse;
   byte success;

   lcd_init();

   while(1)
   {
      t_pulse = measure_pulse(0, 50, &success);		// measure the zero time

      if (success)
      {
		  lcd_clr_line(0);
	      printf(lcd_char, "0 %3.3e", t_pulse);
      }
      else
      {
		  lcd_clr_line(0);
	      printf(lcd_char, "Invalid");
      }

      t_pulse = measure_pulse(1, 50, &success);		// measure the logic one time
      if (success)
      {
		  lcd_clr_line(1);
	      printf(lcd_char, "1 %3.3e", t_pulse);
      }
      else
      {
		  lcd_clr_line(1);
	      printf(lcd_char, "Invalid");
      }
   }
}


float measure_pulse(byte state, byte max_rollovers, byte *p_success)
{

	byte rollovers = 0, is_valid = TRUE;
	unsigned long t1, t2;
	float t_pulse;

	// fire up timer 1
	tmr1cs = 0;						// 1 usec at 4.0 MHz
	t1ckps1 = 0;  t1ckps0 = 0;		// prescale 1:1

	TMR1H = 0x00;
	TMR1L = 0x00;

    if (state)
    {
	   CCP1CON = 0x05;				// interrupt on rising edge
    }
    else
    {
	   CCP1CON = 0x04;				// falling edge

    }

	tmr1if = 0;
	ccp1if = 0;						// kill any old interrupts


	tmr1on = 1;						// get it going
	tmr1_int_occ = FALSE;

	tmr1ie = 1;
	ccp1ie = 1;
	peie = 1;
	gie = 1;

	while(1)
	{
		if (tmr1_int_occ)
		{
			++rollovers;
			if (rollovers == max_rollovers)
			{
				is_valid = FALSE;
				break;
		    }
		    tmr1_int_occ = FALSE;

	    }

	    if (capture_int_occ)
	    {
			rollovers = 0;
			t1 = CCPR1H;			// fetch time t1 for CCP1H & L
			t1 = (t1 << 8) | CCPR1L;
			capture_int_occ = FALSE;
			break;
	    }
    }

    while (gie)	// turn off interrupts for the moment
    {
		gie = 0;
    }

    ccp1m0 = !ccp1m0;			// change CCP mode for interrupt on opposite edge

    ccp1if = 0;
    gie = 1;

    while(1)    				// now for the second transition
    {
		if(!is_valid)
		{
			break;
		}

		if (tmr1_int_occ)
		{
			++rollovers;
			if (rollovers == max_rollovers)
			{
				is_valid = FALSE;
				break;
		    }
		    tmr1_int_occ = FALSE;
	    }
	    if (capture_int_occ)
	    {
			t2 = CCPR1H;		// fetch time t2
			t2 = (t2 << 8) | CCPR1L;
			capture_int_occ = FALSE;
			break;
		}
    }

    while(gie)					// turn off ints
    {
		gie = 0;
    }
    tmr1ie = 0;
	ccp1ie = 0;

    if (is_valid)
    {
	   if(t2 > t1)
       {
	      t_pulse = ((float) rollovers) * 65535.0 + (float) (t2 - t1);
       }
       else
       {
          t_pulse = ((float) rollovers) * 65535.0 - (float) (t1 - t2);
       }
	   *p_success = TRUE;
    }
    else
    {
		*p_success = FALSE;
	}
	return(t_pulse);
}


#int_timer1 timer1_int_handler(void)
{
	tmr1_int_occ = TRUE;
}


#int_ccp1 ccp1_int_handler(void)
{
	capture_int_occ = TRUE;
}

#int_default default_interrupt_handler()
{
}

#include <lcd_out.c>



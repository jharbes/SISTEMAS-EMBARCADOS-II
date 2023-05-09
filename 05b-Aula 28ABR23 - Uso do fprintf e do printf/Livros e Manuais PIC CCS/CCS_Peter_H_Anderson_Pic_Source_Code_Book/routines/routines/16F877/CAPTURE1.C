// Capture1.C
//
// Illustrates the use of Timer1 and Input Capture to continually measure a period
// of an input signal on CCP1/RC2.
//
// In function measure_period, Timer 1 is configured for internal clock, 1:1 prescale.
// Thus, 1 usce per click.  The CCP module is configured for interrupt on rising edge.
//
// If no CCP1 interrupt occurs within the specified number of rollovers of Timer 1
// success is set to FALSE.  Otherwise, the function breaks from the first while(1)
// loop and waits up to the specified number of rollovers for a second CCP1 interrupt.
// The time difference in usecs is returned.
//
// Copyright, Peter H. Anderson, Baltimore, MD, Jan, '01

#case

#device PIC16F877 *=16 ICD=TRUE

#include <defs_877.h>
#include <lcd_out.h>

#define TRUE !0
#define FALSE 0

float measure_period(byte max_rollovers, byte *p_success);

byte tmr1_int_occ, capture_int_occ;

void main(void)
{
   float t_period;
   byte success;

   lcd_init();

   while(1)
   {
      t_period = measure_period(50, &success);
      if (success)
      {
		  lcd_clr_line(0);
		  printf(lcd_char, "%3.3e", t_period);
      }
      else
      {
		  lcd_clr_line(0);
	      printf(lcd_char, "Invalid");
      }
   }
}


float measure_period(byte max_rollovers, byte *p_success)
{
	byte rollovers = 0, is_valid = TRUE;
	unsigned long t1, t2;
	float t_period;

	// fire up timer 1
	tmr1cs = 0;						// 1 usec at 4.0 MHz
	t1ckps1 = 0;  t1ckps0 = 0;		// prescale 1:1

	TMR1H = 0x00;					// set Timer 1 to 0
	TMR1L = 0x00;

	CCP1CON = 0x05;					// interrupt on rising edge

	tmr1if = 0;
	ccp1if = 0;						// kill any old interrupts

	tmr1on = 1;		// get it going
	tmr1_int_occ = FALSE;

	tmr1ie = 1;
	ccp1ie = 1;
	peie = 1;
	gie = 1;

	while(1)
	{
		if (tmr1_int_occ)			// if a timer 1 interrupt
		{
			++rollovers;
			tmr1_int_occ = FALSE;
			if (rollovers == max_rollovers)
			{
				is_valid = FALSE;
				break;
		    }
	    }

	    if (capture_int_occ)		// if an input capture interrupt
	    {
			rollovers = 0;
			t1 = CCPR1H;
			t1 = (t1 << 8) | CCPR1L;
			capture_int_occ = FALSE;
			break;
	    }
    }

    while(1)    // now for the second rising edge
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
			t2 = CCPR1H;			// the value of Timer 1 is stored in CCP1H & L
			t2 = (t2 << 8) | CCPR1L;
			capture_int_occ = FALSE;
			break;
		}
    }

    while(gie)
    {
		gie = 0;
    }
    tmr1ie = 0;
	ccp1ie = 0;

    if (is_valid)
    {
	   if(t2 > t1)
       {
	      t_period = ((float) rollovers) * 65535.0 + (float) (t2 - t1);
       }
       else
       {
          t_period = ((float) rollovers) * 65535.0 - (float) (t1 - t2);
       }
	   *p_success = TRUE;
    }

    else
    {
	   *p_success = FALSE;
	}

	return(t_period);
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

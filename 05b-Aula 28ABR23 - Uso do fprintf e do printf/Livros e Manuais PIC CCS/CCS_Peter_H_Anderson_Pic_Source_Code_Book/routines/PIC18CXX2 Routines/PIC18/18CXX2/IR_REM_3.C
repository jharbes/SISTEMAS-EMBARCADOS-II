// Program IRRem_3.C
//
// This program is the same as IRRem_2.C expept that Timer 3 is used to
// prevent lockup in the input_capture() function.  If a one second timout
// occurs, function input_capture() returns FAILURE and a timeout message is
// displayed to the terminal.
//
//
//      IR Receiver --------------------------- RC2/CCP1 (term 17)
//
//
// copyright, Peter H. Anderson, Baltimore, Dec, '01

#case
#device PIC18C452

#include <defs_18c.h>
#include <delay.h>
#include <ser_18c.h>

#define FALSE 0
#define TRUE !0

#define FAILURE 0
#define SUCCESS !0

#define EXT_32KHZ

#define MAKE_LONG(h, l)  (((long) h) << 8) | (l)

byte fetch_IR_code(void);

byte input_capture(unsigned long *a, byte start_state, byte num_transitions);
void convert_to_widths(unsigned long *a, unsigned long *widths, byte num_ele);
void print_array(unsigned long *a, byte num_ele);
void put_bit(byte *p, byte bit_pos, byte bit_state);

byte flash_led(byte led_num, byte num_flashes, long delay_time);

byte ccp1_int_occ, tmr3_int_occ;


void main(void)
{
    byte code, led_num;
    long delay_time = 200;

    led_num = 0;


	ser_init();
    while(1)
    {
        code = fetch_IR_code();
        printf(ser_char, "Code = %u\r\n", code);
        if (code < 10)
        {
	       flash_led(led_num, code+1, delay_time);	// add one to the key value
	    }

	    else if (code == 16) // up channel
	    {
		   (led_num) ? (led_num = 0) : (led_num = 1);
		   flash_led(led_num, 1, 50); 	// provide some feeback
	    }

	    else if (code == 17)
	    {
		   (led_num) ? (led_num = 0) : (led_num = 1);
		   flash_led(led_num, 1, 50); 	// provide some feeback
	    }

	    else if (code == 18) // up volume
	    {
			delay_time = delay_time - 50;
			if (delay_time < 50)
			{
				delay_time = 50;  // max speed
			}
			flash_led(led_num, 1, 50); 	// provide some feeback
		}

		else if (code == 19) // up volume
		{
			delay_time = delay_time + 50;
			if (delay_time > 500)
			{
				delay_time = 500;  // min speed
			}
			flash_led(led_num, 1, 50); 	// provide some feeback
		}

		else if (code == 0xff)
		{
			printf(ser_char, "\r\nTime Out\r\n");
		}

      else
      {
      }

    }
}

byte fetch_IR_code(void)
{
	unsigned long t_times[18], widths[17];

	byte n, code, valid;

    do
    {
	   if (!input_capture(t_times, 0, 18))	// if failure
	   {
		   return(0xff);
	   }
	   //   print_array(pulses, 18);
	   convert_to_widths(t_times, widths, 17);
	   print_array(widths, 17);

	   valid = TRUE;

	   if (widths[0] < 2000)
	   {
		   valid = FALSE;
	   }

	   for (n=0; n<8; n++)
	   {
		   if ((widths[2*n+1] < 300) || (widths[2*n+1] > 600))
		   {
			   valid = FALSE;
		   }
	   }
    } while(!valid);

	printf(ser_char, "\n\rSuccess\n\r");

	code = 0x00;

	for (n=0; n<7; n++)
	{
	    if (widths[2*(n+1)] < 1000)
	    {
			put_bit(&code, n, 0);
		}
		else
		{
			put_bit(&code, n, 1);
		}
	}

	return(code);
}

byte input_capture(unsigned long *a, byte start_state, byte num_transitions)
{
    byte n;
	// set up timer1

	tmr1cs = 0;	// internal 1 usec clock
	t1ckps1 = 0;  t1ckps0 = 0;
	tmr1on = 1;

	// set up timer 3
	//t3rd16 = 1;
	t3ckps1 = 0;	t3ckps0 = 0;	// 1:1 prescale

   TMR3H = 0x80;	TMR3L = 0x00;   // prime for one second

#ifdef EXT_32KHZ
   t1oscen = 0;
#else
	t1oscen = 1;
#endif
	tmr3cs = 1;	// external clock - 32.768
	tmr3on = 1;

	// set up ccp1 for input capture
	 t3ccp2 = 0;  t3ccp1 = 0;	// timer 1 is clock source for both CCP modules
	ccp1m3 = 0; ccp1m2 = 1; ccp1m1 = 0;  ccp1m0 = start_state;

    // enable interrupts
    ccp1_int_occ = FALSE;
    tmr3_int_occ = FALSE;
    ccp1if = 0;   // kill any pending interrupts
    tmr3if = 0;
 	peie = 1;
	ccp1ie = 1;
	tmr3ie = 1;
	gieh = 1;

	for (n=0; n<num_transitions; n++)
	{
		while(!ccp1_int_occ)
		{
			if (tmr3_int_occ)	// timer 3 is used as a timeout
			{
				while(gieh)
				{
					gieh = 0;
				}
                tmr3on = 0;
				tmr3ie = 0;		// turn off interrupts
				ccp1ie = 0;
				peie = 0;

				tmr3if = 0;
				ccp1if = 0;
				return(FAILURE);
			}
		}

		while(gieh)
		{
			gieh = 0;
		}

	    ccp1_int_occ = FALSE;
		a[n] = MAKE_LONG(CCPR1H, CCPR1L);

		gieh = 1;

	}

	while(gieh)
	{
		gieh = 0;
	}
    tmr3on = 0;
	ccp1ie = 0;
	tmr3ie = 0;
	peie = 0;

	tmr3if = 0;
	ccp1if = 0;
	return(SUCCESS);

}


void convert_to_widths(unsigned long *a, unsigned long *widths, byte num_ele)
{
	byte n;
	for (n=0; n<num_ele; n++)
	{
		if (a[n+1] > a[n])
		{
		   widths[n] = a[n+1] - a[n];
	    }

	    else
	    {
			widths[n] =  a[n+1] - a[n];
		}
	}
}

void put_bit(byte *p, byte bit_pos, byte bit_state)
{
	const byte mask_1[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

	if (bit_state)
	{
		*p = *p | mask_1[bit_pos];
	}

	else
	{
		*p = *p & (~mask_1[bit_pos]);
	}
}


void print_array(unsigned long *a, byte num_ele)
{
	byte n;

	printf(ser_char, "****************\r\n");

	for (n=0; n<num_ele; n++)
	{
		printf(ser_char, "%ld\r\n", a[n]);
	}
}

byte flash_led(byte led_num, byte num_flashes, long delay_time)
{
	const byte mask_1[2] = {0x01, 0x02};
	byte n;

    pspmode = 0;			// portd configured as general purpose IO
	LATD = LATD & 0xfc;
	TRISD = TRISD & 0xfc;	// be sure lower two bits on portd are output zeros
	for (n=0; n<num_flashes; n++)
	{
		LATD = (LATD & 0xfc) | mask_1[led_num];
		delay_ms(delay_time);
		LATD = LATD & 0xfc;
		delay_ms(delay_time);
	}
}


#int_ccp1
ccp1_int_handler(void)
{
	// invert the m0 bit
	ccp1m0 = !ccp1m0;
	ccp1_int_occ = TRUE;
}

#int_timer3
tmr3_int_handler(void)
{
	tmr3_int_occ = TRUE;
}

#int_default
default_int_handler(void)
{
}

#include <delay.c>
#include <ser_18c.c>

// Program IRREM_2.C
//
// Fetches a code from a Sony Remote Control.  If the code is less than 10
// it is interpretted as a number key and an LED is flashed the number of times
// of the key value.  If the code is up volume (18) or down volume (19) the flash
// rate is either increased or decreased.  If the code is up channel (16) or down
// channel(17), a different LED is flashed.
//
//
// This program captures a pulse train and displays the result.
//
//      IR Receiver --------------------------- RC2/CCP1 (term 17)
//
//
// copyright, Peter H. Anderson, Baltimore, Jan, '02

#case
#device PIC18C452

#include <defs_18c.h>
#include <delay.h>
#include <ser_18c.h>

#define FALSE 0
#define TRUE !0

#define MAKE_LONG(h, l)  (((long) h) << 8) | (l)

byte fetch_IR_code(void);

void input_capture(unsigned long *a, byte start_state, byte num_transitions);
void convert_to_widths(unsigned long *a, unsigned long *widths, byte num_ele);
void print_array(unsigned long *a, byte num_ele);
void put_bit(byte *p, byte bit_pos, byte bit_state);

byte flash_led(byte led_num, byte num_flashes, long delay_time);

byte ccp1_int_occ;


void main(void)
{
    byte code, led_num;
    long delay_time = 200;

    led_num = 0;


	ser_init();
    while(1)
    {
        code = fetch_IR_code();
        printf(ser_char, "Code = %d\r\n", code);
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

		else
		{
			// valid code but not defined
		}

    }
}

byte fetch_IR_code(void)
{
	unsigned long t_times[18], widths[17];

	byte n, code, valid;

    do
    {
	   input_capture(t_times, 0, 18);
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

void input_capture(unsigned long *a, byte start_state, byte num_transitions)
{
    byte n;
	// set up timer1
	t1oscen = 0;
	tmr1cs = 0;	// internal 1 usec clock
	t1ckps1 = 0;  t1ckps0 = 0;
	tmr1on = 1;


	// set up ccp1 for input capture
	t3ccp2 = 0;  t3ccp1 = 0;	// timer 1 is clock source for both CCP modules
	ccp1m3 = 0; ccp1m2 = 1; ccp1m1 = 0;  ccp1m0 = start_state;

    // enable interrupts
    ccp1_int_occ = FALSE;
	peie = 1;
	ccp1ie = 1;
	gieh = 1;

	for (n=0; n<num_transitions; n++)
	{
		while(!ccp1_int_occ)	/* loop */  ;
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
	ccp1ie = 0;
	peie = 0;
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

#int_default
default_int_handler(void)
{
}

#include <delay.c>
#include <ser_18c.c>

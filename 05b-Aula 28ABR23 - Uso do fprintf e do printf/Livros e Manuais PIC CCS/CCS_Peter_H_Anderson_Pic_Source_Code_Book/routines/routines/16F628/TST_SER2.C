// TST_SER2.C (PIC16F628)
//
// Illustrates the use of serial utilities in SER_628.C.
//
// Interfaces with BasicX Serial LCD+ but may be modified for use with
// with any serial LCD capable of receiving RS232 non-inverted at 9600
// baud.
//
// PIC16F628					Serial LCD+
//
//  RB2/TX (term 8) ----------> (term 2)
//
// copyright, Peter H. Anderson, Baltimore, MD, April, '01

#case

#device PIC16F628 *=16

#include <defs_628.h>
#include <ser_628.h>
#include <delay.h>

void main(void)
{
	byte bb = 196, n;
	long ll = 1234;
	float ff = 3.2;

	ser_init();
	ser_lcd_init();

	ser_lcd_set_beep_freq(100);

	for (n = 0; n<5; n++)
	{
		ser_lcd_backlight_off();
		ser_lcd_beep();
		delay_ms(200);
		ser_lcd_backlight_on();
		ser_lcd_beep();
		delay_ms(200);
	}

	while(1)	// continually output Hello World, a byte,
				// long and float
	{
		ser_lcd_beep();
        ser_lcd_clr_all();
        ser_lcd_cursor_pos(0, 0);
        printf(ser_char, "Hello World");
	    ser_lcd_cursor_pos(0, 1);
	    printf(ser_char, "%3.2f ", ff);

        ser_dec_byte(ll/100, 2);
        ser_dec_byte(ll%100, 2);

	    ser_lcd_cursor_pos(0, 2);
	    printf(ser_char, "%u %x", bb, bb);

        ++ll;
        ++bb;
	    delay_ms(500);

        trisb0 = 0;		// blip an LED on and off
        rb0 = 1;
        delay_ms(200);
        rb0 = 0;
        delay_ms(200);
	}
}

#include <ser_628.c>
#include <delay.c>

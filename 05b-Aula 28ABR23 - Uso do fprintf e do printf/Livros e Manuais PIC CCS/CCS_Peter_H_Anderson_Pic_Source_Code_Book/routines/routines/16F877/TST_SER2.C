// TST_SER2.C
//
// Illustrates the use of ser_87x.c utility routines.
//
// Initializes UART and initializes Serial LCD (BasicX Serial LCD+)
// Continually displays "Hello World" and a byte in both decimal and
// hex formats, a long and a float.
//
// PIC16F877			Serial LCD+
//
// RC6/TX (term 25) -------> (term 2)
//
// copyright, Peter H. Anderson, Baltimore, MD, Apr, '01

#case

#device PIC16F877 *=16 ICD=TRUE

#include <defs_877.h>
#include <ser_87x.h>
#include <delay.h>

void main(void)
{
   byte bb = 196, n;
   long ll = 1234;
   float ff = 3.2;

   ser_init();
   ser_lcd_init();

   ser_lcd_set_beep_freq(100);

   for (n = 0; n<5; n++)	// attract some attention by flashing and beeping
					// LCD
   {
      ser_lcd_backlight_off();
 	  ser_lcd_beep();
	  delay_ms(200);
	  ser_lcd_backlight_on();
	  ser_lcd_beep();
	  delay_ms(200);
   }

   while(1)
   {
      ser_lcd_beep();
      ser_lcd_clr_all();
      ser_lcd_cursor_pos(0, 0);
      printf(ser_char, "Hello World");
      ser_lcd_cursor_pos(0, 1);
      printf(ser_char, "%3.2f ", ff);	// display a float

      ser_dec_byte(ll/100, 2);		// display a long
      ser_dec_byte(ll%100, 2);

      ser_lcd_cursor_pos(0, 2);
	  printf(ser_char, "%u %x", bb, bb);

      ++ll;				// modify the values
      ++bb;
 	  delay_ms(500);
   }
}

#include <ser_87x.c>
#include <delay.c>


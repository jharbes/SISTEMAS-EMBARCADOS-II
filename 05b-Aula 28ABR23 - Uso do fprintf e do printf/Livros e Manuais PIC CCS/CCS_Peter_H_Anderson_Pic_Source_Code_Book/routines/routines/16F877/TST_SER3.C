// Program TST_SER3.C
//
// Illustrates output to serial LCD and input from BX24.
//
// BX24					PIC16F877				Serial LCD+
// (term 12) ---------> RC7/RX (term 26)
//                  	RC6/TX (term 25) ------> (term 2)
//
// Fetches character string until the character 13 (0x0d)
// is received and then outputs the string to the serial LCD.
//
// Fetches character string until five characters are recieved
// displays on the serial LCD.
//
// copyright, Peter H. Anderson, Baltimore, MD, Mar, '01

#case

#device PIC16F877 *=16 ICD=TRUE

#include <defs_877.h>
#include <ser_87x.h>
#include <delay.h>
#include <string.h>

void set_next_line(byte *p_line);

void main(void)
{
   char s[20];
   byte n, line = 0, num_chars;

   ser_init();
   ser_lcd_init();

   while(1)
   {

      if((num_chars = ser_get_str_1(s, 10000, 1000, 13)) !=0)
      				// up to 10 secs on first
					// 1 sec on subsequent
					// until new line char
      {
	     ser_lcd_clr_line(line);
	     printf(ser_char, "%d ", num_chars);
         ser_out_str(s);
	  }
	  else
	  {
		 ser_lcd_clr_line(line);
		 printf(ser_char, "NULL");
	  }
	  set_next_line(&line);

      delay_ms(500);

      if((num_chars = ser_get_str_2(s, 10000, 1000, 5)) !=0)
      				// up to 10 secs on first
					// 1 sec on subsequent
					// first five characters
      {
	     ser_lcd_clr_line(line);
	     printf(ser_char, "%d ", num_chars);
         ser_out_str(s);
	  }
	  else
	  {
		 ser_lcd_clr_line(line);
		 printf(ser_char, "NULL");
	  }
	  set_next_line(&line);
      delay_ms(500);
   }
}

void set_next_line(byte *p_line)
{
	++(*p_line);
	if (*p_line == 4)
	{
		*p_line = 0;
	}
}

#include <ser_87x.c>
#include <delay.c>

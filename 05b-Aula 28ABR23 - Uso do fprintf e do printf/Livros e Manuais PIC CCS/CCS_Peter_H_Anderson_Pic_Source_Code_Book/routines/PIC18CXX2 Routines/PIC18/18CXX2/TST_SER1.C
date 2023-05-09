// TST_SER1.C
//
// Illustrates the use of ser_18c.c utility routines.
//
// Initializes UART and continually displays "Hello World" and a byte in
// both decimal and hex formats, a long and a float.
//
// PIC16C452					  PC COM Port
//
// RC6/TX (term 25) ---- DS275 ---> (term 2)
//
// copyright, Peter H. Anderson, Baltimore, MD, Dec, '01


#case
#device PIC18C452

#include <defs_18c.h>
#include <delay.h>
#include <ser_18c.h>

void main(void)
{
   byte bb = 196, n;
   long ll = 1234;
   float ff = 3.2;

   ser_init();		// configures UART

   while(1)
   {

      printf(ser_char, "Hello World\r\n");
      printf(ser_char, "%3.2f\r\n", ff);	// display a float

      ser_dec_byte(ll/100, 2);		// display a long using ser_dec_byte
      ser_dec_byte(ll%100, 2);
      ser_newline();

	  printf(ser_char, "%u %x\r\n", bb, bb);

      ++ll;				// modify the values
      ++bb;
 	  delay_ms(500);
   }
}

#include <delay.c>
#include <ser_18c.c>



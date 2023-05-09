// BITS_1.C
//
// Illustrates the use of bit fields.
//
// Configuration - LEDS (8) on PORTD0 - PORTD7
//
// Each 250 ms, inverts the state of the LED on PORTD4 and displays a three bit
// count (0 - 5) on PORTD5::7.  At the end of the sequence, the number of times the
// for loop has been executed is displayed on LEDs on bits 0 - 3.
//
// Copyright, Peter H. Anderson, Baltimore, MD, Jan, '01

#case

#device PIC16F877 *=16 ICD=TRUE

#include <defs_877.h>
#include <lcd_out.h>

#define TRUE !0
#define FALSE 0

struct PORTD_BITS
{
   byte FOUR_LEDS : 4;     	// bits 0 - 3
   byte ONE_LED : 1;     	// bit 4
   byte THREE_LEDS: 3;		// bits 5, 6, 7
};

struct PORTD_BITS portd_bits;  // global as opposed to passing to functions

void main(void)
{
   int m = 0, n;

   pspmode = 0;

   portd_bits = 0x00;
   PORTD = portd_bits;

   TRISD = 0x00;		// all outputs

   while(1)
   {
      for (n=0; n<6; n++)
      {
	     portd_bits.ONE_LED = !portd_bits.ONE_LED;	// toggle bit 4
	     portd_bits.THREE_LEDS = n;
	     PORTD = portd_bits;
	     delay_ms(250);
	}
      portd_bits.FOUR_LEDS = m;  // display the number of times the loop is executed
      PORTD = portd_bits;

      ++m;
	if (m == 16)	// limit the variable to 0 - 15
      {
         m = 0;
      }
   }
}

#include <lcd_out.c>

// FLASH_1.C (PIC16F628)
//
// Flashes an LED on PORTB.7 when pushbutton on PORTB.0 is depressed.
//
// copyright, Peter H. Anderson, Baltimore, MD, Apr, '01

#case

#device PIC16F628 *=16

#include <defs_628.h>
#include <delay.h>

#define FALSE 0
#define TRUE !0

void main(void)
{
    rb7 = 0;
	trisb7 = 0;		// make LED an output 0
	not_rbpu = 0;	// enable weak pullups

    while(1)		// continually
	{
	   while(!rb0)	// if at logic zero
	   {
	   	   rb7 = 1;
		   delay_ms(200);
		   rb7 = 0;
		   delay_ms(200);
	   }
	}
}

#include <delay.c>

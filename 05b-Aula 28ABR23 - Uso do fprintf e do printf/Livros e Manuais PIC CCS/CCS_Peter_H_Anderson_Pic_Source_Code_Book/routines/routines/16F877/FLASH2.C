// FLASH2.C
//
// Same as FLAS1.C except that timing routines are located in lcd_out.h
// and lcd_out.c
//
// Continually flashes an LED on PORTD.4 in bursts of five flashes.
//
// This is intended as a demo routine in presenting the various features
// of the Serial In Circuit Debugger.
//
// Although this was written for a 4.0 MHz clock, the hex file may be used
// with a target processor having 8.0, 10.0 or 20.0 MHz clock.  Note that
// the time delays will be 2, 2.5 and 5 times faster.
//
// copyright, Peter H. Anderson, Baltimore, MD, Dec 14, '00
//

#case

#device PIC16F877 *=16 ICD=TRUE

#include <defs_877.h>
#include <lcd_out.h>

void flash(byte num_flashes);

void main(void)
{
   while(1)
   {
     pspmode = 0;    // make PORTD general purpose IO
     flash(5);
     delay_ms(3000);
   }
}

void flash(byte num_flashes)
{
   byte n;
   for (n=0; n<num_flashes; n++)
   {
     trisd4 = 0;        // be sure bit is an output
     portd4 = 1;
     delay_ms(500);
     portd4 = 0;
     delay_ms(500);
   }
}

#include <lcd_out.c>
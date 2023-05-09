// FLASH1.C
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

void flash(byte num_flashes);
void delay_10us(byte t);
void delay_ms(long t);

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

void delay_10us(byte t)
// provides delay of t * 10 usecs (4.0 MHz clock)
{
#asm
      BCF STATUS, RP0
DELAY_10US_1:
      CLRWDT
      NOP
      NOP
      NOP
      NOP
      NOP
      NOP
      DECFSZ t, F
      GOTO DELAY_10US_1
#endasm
}

void delay_ms(long t)   // delays t millisecs (4.0 MHz clock)
{
   do
   {
     delay_10us(100);
   } while(--t);
}





// FLASH_1.C
//
// Flashes LED on PORTD0 (term 19) when PORTB7 (term 40) is at ground.
//
// Uses LAT instruction.
//
// copyright, Peter H. Anderson, Baltimore, MD, Jan, '02
//

#case
#device PIC18C452


#include <defs_18c.h>
#include <delay.h>

void flash(byte t_ms);

void main(void)
{
	pspmode = 0;		// use portd as general purpose IO
	latd0 = 0;
	trisd0 = 0;

	not_rbpu = 0;		// enable weak pullup resistors on portb

	while(1)
	{
       if(!portb7)
       {
          flash(50);
       }
       else
       {
		   latd0 = 0;
	   }
    }
}

void flash(byte t_ms)
{
   latd0 = 1;
   delay_ms(t_ms);
   latd0 = 0;
   delay_ms(t_ms);
}

#include <delay.c>


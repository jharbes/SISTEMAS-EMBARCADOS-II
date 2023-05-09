// SW_CLK.C
//
// Flashes LED on portd0 (term 19) five times using the 4.0 MHz system clock and then
// flashed LED on portd1 (term 20) three times using the 32.768 kHz clock.
//
//
// copyright, Peter H. Anderson, Baltimore, MD, Jan, '02
//

#case
#device PIC18C452

#include <defs_18c.h>
#include <delay.h>

#define FALSE 0
#define TRUE !0

void flash_portd0(byte num_flashes);
void flash_portd1(byte num_flashes);
void delay_ms_32khz(long t);

void main(void)
{
	byte n;

	pspmode = 0;		// configure PORTD as
	t1oscen = 1;		// enable external crystal osc

	scs = 0;	// use 4.0 MHz clock

    while(1)
    {
		
           flash_portd0(5);
           delay_ms(1000);
	   

	    scs = 1;	// system clock switch to timer 1
      
			flash_portd1(3);
			delay_ms_32khz(1000);
		
		scs = 0;	// back to 4.0 MHz XT clock

    }
}

void flash_portd0(byte num_flashes)
{
   byte n;
   for (n=0; n<num_flashes; n++)
   {
     trisd0 = 0;
     latd0 = 1;
     delay_ms(200);
     latd0 = 0;
     delay_ms(200);
   }
}

void flash_portd1(byte num_flashes)
{
   byte n;
   for (n=0; n<num_flashes; n++)
   {
     trisd1 = 0;
     latd1 = 1;
     delay_ms_32khz(200);
     latd1 = 0;
     delay_ms_32khz(200);
   }
}

void delay_ms_32khz(long t)
{
	byte i;

	while(t--)    // 10 ~  or about 1.22 ms
	{

   }
}

#include <delay.c>


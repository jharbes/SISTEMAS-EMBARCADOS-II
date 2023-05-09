// Program EXT_INT1.C
//
// Illustrates the use of external interrupts on INT0/RB0, INT1/RB1 and
// INT2/RB2.
//
// All interrupts are configured for rising edge.
//
// On interrupt, LED on PORTD0 is flashed at various speeds, depending on the
// interrupt source.
//
// copyright, Peter H Anderson, Baltimore, MD, Jan, '02

#case
#device PIC18C452

#include <defs_18c.h>
#include <delay.h>
#include <ser_18c.h>

#define FALSE 0
#define TRUE !0

void flash_led(byte num_flashes, long delay_time);

byte int0_occ, int1_occ, int2_occ;	// note globals

void main(void)
{
   not_rbpu = 0;	// internal pullups

   pspmode = 0;		// configure PORTD for general purpose IO

   latd0 = 0;		// LED is an output
   trisd0 = 0;

   int0_occ = FALSE;  int1_occ = FALSE;  int2_occ = FALSE;

   intedg0 = 1;  intedg1 = 1;  intedg2 = 1;	// interrupt on rising edge
   ipen = 0;	// disable interrupt priority mode

   while(1)
   {
	  int0ie = 1;  int1ie = 1;  int2ie = 1;	// enable ints
	  gieh = 1;

      if (int0_occ)
      {
		 while(gieh)
		 {
			gieh = 0;
		 }
         int0_occ = FALSE;

         flash_led(5, 200);
         int0if = 0;
         gieh = 1;

	  }

      if (int1_occ)
      {
		 while(gieh)
		 {
			gieh = 0;
		 }
         int1_occ = FALSE;

         flash_led(5, 100);
         int1if = 0;
         gieh = 1;

	  }

      if (int2_occ)
      {
		 while(gieh)
		 {
			gieh = 0;
		 }
         int2_occ = FALSE;

         flash_led(5, 50);
         int2if = 0;
         gieh = 1;

	  }
   }
}

void flash_led(byte num_flashes, long delay_time)
{
	byte n;

	for (n=0; n<num_flashes; n++)
	{
		latd0 = 1;
		delay_ms(delay_time);
		latd0 = 0;
		delay_ms(delay_time);
	}
}

#int_ext
int0_int_handler(void)
{
   int0_occ = TRUE;
}

#int_ext1
int1_int_handler(void)
{
   int1_occ = TRUE;
}

#int_ext2
int2_int_handler(void)
{
   int2_occ = TRUE;
}

#int_default
default_int_handler(void)
{
}

#include <delay.c>

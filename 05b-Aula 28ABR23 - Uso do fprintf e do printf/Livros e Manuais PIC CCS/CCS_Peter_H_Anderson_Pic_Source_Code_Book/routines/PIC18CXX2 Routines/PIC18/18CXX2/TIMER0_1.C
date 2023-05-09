// Program TIMER0_1.C
//
// Illustrates the use of Timer 0 in the 16-bit mode to time for very close to one second.
//
// Each second, toggles LED on PORTD0.
//
// copyright, Peter H. Anderson, Baltimore, MD, Jan, '02

#case
#device PIC18C452

#include <defs_18c.h>
#include <delay.h>
#include <ser_18c.h>

#define FALSE 0
#define TRUE !0

#define MAKE_LONG(h, l)  (((long) h) << 8) | (l)

main()
{
   ser_init();	// for possible debuuging

   pspmode = 0;
   latd0 = 0;    // make LED an output logic zero
   trisd0 = 0;

   // Set up timer0
   t0cs = 0;	// fosc/4 is source
   psa = 0;		// prescaler assigned to TMR0

   t0ps2 = 1; t0ps1 = 1; t0ps0 = 1;	// prescale of 1:256
   t08bit = 0;	// configure for 16-bit mode

   TMR0H = (byte) ((~3906 + 1) >> 8);
   TMR0L = (byte) (~3906 + 1);

   tmr0on = 1;

   t0if = 0;	// kill any pending interrupt
   t0ie = 1;
   gieh = 1;

   while(1)
   {
   }
}

#int_timer0
timer0_int_handler(void)
{
	byte h, l;
	unsigned long t_new;

	latd0 = !latd0;

	l = TMR0L;		// important to read LSByte first
	h = TMR0H;

	t_new = MAKE_LONG(h, l) + (~3906 + 1);

	TMR0H = (byte)(t_new >> 8);	// write high byte first
    TMR0L = (byte)(t_new);
}


#int_default
default_int_handler(void)
{
#asm
   NOP		// for debugging
   NOP
#endasm
}

#include <delay.c>
#include <ser_18c.c>

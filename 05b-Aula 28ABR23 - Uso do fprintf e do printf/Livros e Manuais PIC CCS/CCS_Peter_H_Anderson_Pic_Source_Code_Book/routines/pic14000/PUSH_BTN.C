// Program PUSH_BTN.C (PIC14000)
//
// copyright, Peter H. Anderson, Elmore, VT, July, '02


#case

#device PIC14000 *=16

#include <defs_14.h>
#include <delay.h>

#define TRUE !0
#define FALSE 0


byte pb_int_occ, t0_num_ov;	// for communicating with interrupt service routine


void main(void)
{

   portc4 = 0;
   trisc4 = 0;	// make RC4 an output logic zero

   inclken = 0;	// disconnect internal clock from term OSC2
   osc2 = 0;	// logic zero on output OSC2

   // fire up timer 0
   t0_num_ov = 0;
   ;
   t0cs = 0;
   psa = 0;	// assign prescaler to timer 0
   ps2 = 0;
   ps1 = 1;
   ps0 = 1;	// 16 usec	- ~ 4 ms per rollover

   TMR0 = 0x00;
   t0if = 0;
   t0ie = 1;

   // configure push button interrupt

   pbif = 0;
   pbie = 1;

   peie = 1;
   gie = 1;

   while(1)
   {
	   if (pb_int_occ)
	   {
	       pb_int_occ = FALSE;

		   pbie = 0;	// disable the interrupt during debouce
		   while(pbif)
		   {
			   pbif = 0;
		   }
		   osc2 = !osc2;
		   delay_ms(50);	// debounce
		   pbif = 0;
		   pbie = 1;
	   }
	}
}

#int_button
push_button_int_handler(void)
{
   pb_int_occ = TRUE;
}

#int_rtcc
timer0_int_handler(void)
{
	++t0_num_ov;
	if (t0_num_ov > 100)
	{
		 t0_num_ov = 0;
		 portc4 = !portc4;
	}
}

#int_default
default_int_handler(void)
{
}

#include <delay.c>



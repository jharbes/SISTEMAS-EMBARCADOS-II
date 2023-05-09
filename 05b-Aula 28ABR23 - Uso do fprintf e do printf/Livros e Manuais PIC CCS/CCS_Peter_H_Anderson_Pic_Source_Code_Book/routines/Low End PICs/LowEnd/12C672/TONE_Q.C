// TONE_Q.C (PIC12C672), CCS PCM
//
// Intended for possible use with frost alarm in place of serial output
// to serial LCD or to PC Com Port.
//
// When input GP3 is at ground, T_threshold is sounded on speaker on output
// GP1.  When input GP3 is not at ground, the current value of T_C is output on
// the speaker.
//
// In sounding the quantity, a long 500 Hz tone indicates a minus.  Each digit is
// sounded as a series of 250 ms beeps.
//
//
//  GRD --- \---- GP3 (internal weak pull-up)
//
//                GP1 ---------||--- SPKR --- GRD
//                           + 47 uFd
//
// Use internal RC oscillator.
//
// copyright, Peter H. Anderson, Elmore, VT, July, '01

#case

#device PIC12C672

#include <defs_672.h>
#include <delay.h>

#define TRUE !0
#define FALSE 0

void beep(long ms);
void beep_q(byte q, byte minus_flag);
void calibrate(void);

long ms_count;  // used in TMR0 ISR

void main(void)
{
	byte T_threshold = 34, T_C, minus_flag, n;
	char const T_C_array[5] = {-5, 0, 1, 25, 70};

	calibrate();

    pcfg2 = 1; // configure A/D for AN0 (GP0) - Not used in this example
    pcfg1 = 1; // others as IO
    pcfg0 = 0;

	while(1)
	{
       not_gppu = 0;
	   if(!gp3)	// if switch at ground
	   {
	 	  beep_q(T_threshold, FALSE);
	   }

	   else
	   {
		   for (n = 0; n< 5; n++)
		   {
			   T_C = T_C_array[n];
			   if (T_C < 0)
			   {
				   minus_flag = TRUE;
				   T_C = (~T_C) + 1;
			   }
			   else
			   {
				   minus_flag = FALSE;
			   }
			   beep_q(T_C, minus_flag);
		   }
	   }
      delay_ms(1000);
   }
}

void beep_q(byte q, byte minus_flag)
{
   byte n, digit;


   if (minus_flag)
   {
	   beep(500);
	   delay_ms(500);
   }

   digit = q/10;	// number of tens
   if (digit)	    // if non zero
   {
	   for (n=0; n<digit; n++)
	   {
		   beep(250);
		   delay_ms(250);
	   }

	   delay_ms(500);	// separation between digits
   }

   digit = q%10;
   if (!digit)
   {
	   digit = 10;
   }

   for (n=0; n<digit; n++)
   {
   	   beep(250);
   	   delay_ms(250);
   }

   delay_ms(500);	// separation between digits
}

void beep(long ms)
{
	gp1 = 0;
	tris1 = 0;

	// configure TMR0
	t0cs = 0;	// use internal f_osc
	ps2 = 0;	// prescale 1:4, thus, rollover every 256 * 4 usec
	ps1 = 0;
	ps0 = 0;
	psa = 1;
	t0if = 0;	// kill any pending interrupt;
	t0ie = 1;	// enable TMR0 interrupt
	gie = 1;

    ms_count = ms;
	while(ms_count)
	{
#asm
        CLRWDT
#endasm
   }

	while(gie)
	{
		gie = 0;
	}

	t0ie = 0;	// book keeping
	t0if = 0;

	gp1 = 0;
}

void calibrate(void)
{
#asm
   CALL 0x03ff
   MOVWF OSCCAL
#endasm
}

#int_rtcc tmr0_int_handler(void)
{
	gp1 = !gp1;
	--ms_count;
}

#int_default default_int_handler(void)
{
}

#include <delay.c>

// Program MUSIC_1.C
//
// Plays a musical sequence on speaker on PORTB3.
//
// Uses CCP2 and Timer 3 in the Trigger Special Event mode to toggle PORTB
// at the frequency of the musical note and uses CCP1 and Timer 1 in Trigger
// Special Event to provide the timing for the duration of the note.
//
// Also illustrates the use of enum.
//
// copyright, Peter H. Anderson, Baltimore, MD, Jan, '02

#ifdef XXX

Musical Notes
	freq		Period (us)	1/2 Period (us)

E3	164.8		6067.961165	3033.980583
F3	174.6		5727.376861	2863.688431
G3	196.00  	5102.040816	2551.020408
A3	220.00  	4545.454545	2272.727273
B3	246.94		4049.566696	2024.783348
C3	261.63		3822.191645	1911.095822
D3	293.66		3405.298645	1702.649322
E4	329.63		3033.704457	1516.852228
F4	349.23		2863.44243	1431.721215

#endif

#case
#device PIC18C452

#include <defs_18c.h>
#include <delay.h>
#include <ser_18c.h>

byte num_12_5ms;

void main(void)
{
   enum note {E3, F3, G3, A3, B3, C3, D3, E4, F4};
   enum dur {Whole, Half, Quarter, Eighth, Sixteenth};

   const long half_periods[9] = {3034, 2864, 2551, 2273, 2025. 1911, 1703, 1517, 1432};
   const long durations[5] = {80, 40, 20, 10, 5};

   const byte notes_f[5] = {E3, A3, C3, D3, E4};
   const long notes_d[5] = {Whole, Half, Whole, Eighth, Whole};

   byte n;

   ser_init();


	// set up timer3
	t3rd16 = 0;
	t3ckps1 = 0;	t3ckps0 = 0;	// 1:1 prescale
	tmr3cs = 0;	// internal clock - 1 usec per tick
   	TMR3H = 0x00; TMR3L = 0x00;

   	// setup CCP2
   	t3ccp2 = 0;		t3ccp1 = 1;	// assign timer3 to CCP2, timer1 to CCP1
	ccp2m3 = 1;  ccp2m2 = 0; ccp2m1 = 1;  ccp2m0 = 1;	// special event - resets Timer 3

	// set up timer 1

	t1rd16 = 0;
	t1ckps1 = 0;	t1ckps0 = 0;	// 1:1 prescale
	t1oscen = 0;
	tmr1cs = 0;

	TMR1H = 0x00;	TMR1L = 0x00;

	// set up CCP1
	ccp1m3 = 1;  ccp1m2 = 0; ccp1m1 = 1;  ccp1m0 = 1;	// special event - resets Timer 1

	for (n=0; n<5; n++)
	{
		// disable interrupts
		while(gieh)
		{
			gieh = 0;
		}
		ccp1ie = 0;
		ccp2ie = 0;
		tmr3on = 0;
		tmr1on = 0;

		CCPR2H = (byte) (half_periods[notes_f[n]] >> 8);
		CCPR2L = (byte) (half_periods[notes_f[n]]);

		CCPR1H = (byte) (12500 >> 8);	// 12.5 ms
		CCPR1L = (byte) (12500);

		num_12_5ms = durations[notes_d[n]];

	    tmr3on = 1;
        tmr1on = 1;

	    // enable interrupts

	    ccp1if = 0;
	    ccp2if = 0;

	    peie = 1;
	    ccp1ie = 1;
	    ccp2ie = 1;
	    gieh = 1;

	    while(num_12_5ms)
	    {
			/* loop until zero */
		}
	}

// disable interrupts
	while(gieh)
	{
		gieh = 0;
	}
	ccp1ie = 0;
	ccp2ie = 0;
	tmr3on = 0;
	tmr1on = 0;

	while(1)
	{
		/* loop to keep emulator from stalling */
	}
}

#int_ccp1
compare1_int_handler(void)
{
   --num_12_5ms;
}

#int_ccp2
compare2_int_handler(void)
{
   trisb3 = 0;			// toggle portb3
   latb3 = latb3;
}

#int_default
default_handler(void)
{
}

#include <delay.c>
#include <ser_18c.c>


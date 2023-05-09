#ifdef DESCRIPTION
Flasher Alarm

This is a simple design to alert a driver in a noisy truck when a turn signal has been left on for an extended period of time.

Normally, the +12V for the flashers on each side of the vehicle is at an open and thus the input to the onboard counter (TMR0) is stable at a logic zero (100K pulldown resistor) and the program loops, delaying for nominally one second and then testing to see if the count in TMR0 is zero.

When either flasher is activated, the count in TMR0 over a one second period is not zero and the program exits the "no signal condition loop" and enters a second loop which similarly delays for nominally one second and if the count in TMR0 over the course of a second is non zero, variables secs is incremented.

When variable secs is greater than a defined TIMEOUT, the program exits this second loop and enters a third loop which continually beeps a speaker at 500 Hz for nominally one second.

Note that if the signal condition disappears (TMR0 at zero over one second), the program returns to the "no signal present" loop using a GOTO TOP.

There are flaws in the design.  For example, if the driver has the right directional on for 35 seconds and switches to the left signal, TIMEOUT may well be exceeded and an alarm will be generated.  However, the alarm is simply a noise maker, not an ejection seat, and the alarm will be terminated when the driver turns off the left signal.

A second flaw is that this system has no way of distinguishing directional signals from emergency flashers where the flashers may well be on for longer than TIMEOUT.  However, again, the alarm is simply noise.

Note that although the PIC16HV540 is designed for +12V operation, the signal at the T0CK1 input is to be 5V logic.  Thus, a 22K series limiting diode is used.  Note that the right and left flasher signals are "ored" using diodes.

This program uses nominally 135 of 512 program words in the PIC16HV540.

#endif

// FLSH_ALM.C (PIC16HV540)
//
//            PIC16HV540
//
// Right Flash ----->|---- 22K -------- T0CK1 (term 3)
// Left Flash ------>|
//                   |
//                  100K            PORTB0 (term 6) -------|(---- SPKR ---- GRD
//      |           									+ 47 uFd
//     GRD
//
// copyright, Peter H. Anderson, Elmore, VT, July, '01

#case

#device PIC16HV540

#include <defs_540.h>
#include <delay.h>

#define TIMEOUT 45  // adjust timeout in seconds as neccessary

void beep_sec(void);
void setup_tmr0(void);
void config_processor(void);

void main(void)
{
 byte secs;

TOP:
   while(1)  // "no turn signal" condition
   {
#asm
      CLRWDT
#endasm
   setup_tmr0();
      TMR0 = 0x00;
      secs = 0;
      delay_ms(1000);
      if (TMR0 != 0)
      {
    break; // turn signal is on
   }
   }

   while(1)  // "turn signal on" condition
   {
    setup_tmr0();
#asm
       CLRWDT
#endasm
    TMR0 = 0x00;
    delay_ms(1000);
    if (TMR0 == 0) // condition is not present
       {
    goto TOP;
    }

    // else
    ++secs;
    if (secs > TIMEOUT)
    {
       break; // go to alarm state
    }
   }

   while(1)  // alarm condition
   {
    setup_tmr0();
#asm
      CLRWDT
#endasm
    TMR0 = 0x00;
    beep_sec();  // beep for one second
       if (TMR0 == 0) // condition is no longer present
       {
    goto TOP;
    }
    // else, stay in the loop
   }
}

void beep_sec(void)
{
 byte n;
 dirb0 = 0;
#asm
 MOVF DIRB, W // make rb0 an output
 TRIS PORTB
#endasm
    for (n = 0; n<125; n++)  // beep 500 Hz for 250 ms
    {
    portb0 = 1;
    delay_ms(1);
    portb0 = 0;
    delay_ms(1);
    }

    delay_ms(250);

    for (n = 0; n<125; n++)  // beep 500 Hz for 250 ms
    {
    portb0 = 1;
    delay_ms(1);
    portb0 = 0;
    delay_ms(1);
    }

 delay_ms(250);  // and off for 250 ms
}

void setup_tmr0(void) // configure OPTION2 register
{
 t0se = 1; // rising edge
 t0cs = 1; // source is external T0CK1
 psa = 1; // prescale assigned to WDT
    ps2 = 0; // prescale for WDT set to 1:1
    ps1 = 0;
    ps0 = 0;
#asm
    MOVF OPTIONS1, W
 OPTION
#endasm
}

void config_processor(void) // configure OPTION2 registers
{
    not_pcwu = 0; // wakeup enabled
    not_swdten = 1;
    rl = 1;   // regulated voltage is 5V
    sl = 1;   // sleep level same as RL
    not_boden = 1; // brownout disabled
#asm
    MOVF OPTIONS2, W
    TRIS 0x07
#endasm
}

#include <delay.c>

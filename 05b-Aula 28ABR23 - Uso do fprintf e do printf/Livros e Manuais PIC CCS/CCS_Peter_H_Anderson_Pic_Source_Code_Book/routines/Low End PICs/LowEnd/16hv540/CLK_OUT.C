// CLK_OUT.C (PIC16HV540), CCS PCB
//
// Illustrates the use of CLKOUT to gate tones to "sound out" a quantity.
// Note that the RC Timer configuration on OSC1.  I used R = 22K and C = 100 pFd -
// f_osc is nominally 1.0 MHz at +12 VDC.  The TMR0 prescaler is set for a prescale
// of 1:256.  Thus, f_out = f_osc / 4 / 256 or about 1000 Hz.
//
// When input PORTB7 is at ground, T_threshold is sounded on speaker on output
// CLKOUT.  When input PORTB7 is not at ground, the current value of T_C is output on
// the speaker.
//
// In sounding the quantity, a long 500 Hz tone indicates a minus.  Each digit is
// sounded as a series of 250 ms beeps with an interdigit delay of 1 second.
//
// PIC16HV540
//
// OSC2/CLKOUT (term 15) -------------|(------ SPKR ----- GRD
//    							   + 47 uFd
//					+ 12 VDC
//					  |
//					 10K
//					  |
//  GRD -------- \---------- PORTB7 (term 13)
//
// copyright, Peter H. Anderson, Elmore, VT, July, '01

#case

#device PIC16HV540

#include <defs_540.h>
#include <delay.h>

#define TRUE !0
#define FALSE 0

#define ON !0
#define OFF 0

void config_processor(void);
void beep(byte state);
void beep_q(byte q, byte minus_flag);

void main(void)
{
    byte T_threshold = 34, T_C, minus_flag, n;
    char const T_C_array[5] = {-5, 0, 1, 25, 70};

    DIRA = 0x0f;
    DIRB = 0xff;

    config_processor();

    while(1)
    {

       if(!portb7) // if switch at ground
       {
          beep_q(T_threshold, FALSE);
       }

       else
       {
          for (n = 0; n< 5; n++)
          {
              T_C = T_C_array[n];
              if (T_C & 0x80)		// negative
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
      delay_ms(5000/4);
   }
}

void beep_q(byte q, byte minus_flag)
{
   byte n, digit;

   if (minus_flag)
   {
      beep(ON);
      delay_ms(500/4); // long delay to indicate minus
      beep(OFF);
      delay_ms(500/4);
   }

   digit = q/10; // number of tens
   if (digit)     // if non zero
   {
      for (n=0; n<digit; n++)
      {
         beep(ON);
         delay_ms(250/4);
         beep(OFF);
         delay_ms(250/4);
      }

      delay_ms(1000/4); // separation between digits
   }

   digit = q%10;
   if (!digit)
   {
      digit = 10;
   }

   for (n=0; n<digit; n++)
   {
       beep(ON);
       delay_ms(250/4);
       beep(OFF);
       delay_ms(250/4);
   }

   delay_ms(1000/4); // separation between digits
}

void beep(byte state)
{
   if (state == ON)
   {
       t0cs = 0; // internal clock enabled
   }
   else
   {
       t0cs = 1;
   }
   ps2 = 1;  // prescale of 1:512 plus divide by 2 in TMR0
   ps1 = 1;
   ps0 = 1;
   psa = 0;
#asm
   MOVF OPTIONS1, W
   OPTION
#endasm
}

void config_processor(void) // configure OPTION2 registers
{
    not_pcwu = 1; // wakeup disabled
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

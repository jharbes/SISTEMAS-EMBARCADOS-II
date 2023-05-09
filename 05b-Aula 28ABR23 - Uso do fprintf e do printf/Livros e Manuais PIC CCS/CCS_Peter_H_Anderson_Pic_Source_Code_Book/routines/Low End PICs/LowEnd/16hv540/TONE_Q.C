// TONE_Q.C (PIC16HV540), CCS PCB
//
// Intended for possible use with frost alarm in place of serial output
// to serial LCD or to PC Com Port.
//
// When input PORTB7 is at ground, T_threshold is sounded on speaker on output
// PORTB0.  When input PORTB7 is not at ground, the current value of T_C is output on
// the speaker.
//
// In sounding the quantity, a long 500 Hz tone indicates a minus.  Each digit is
// sounded as a series of 250 ms beeps with an interdigit delay of 1 second.
//
//    +12 VDC
//      |
//     10K
//      |
//  GRD --- \-------------- PORTB7 (term 13)
//
//                          PORTB0 (term 6) ---------||--- SPKR --- GRD
//                                                 + 47 uFd
//
// copyright, Peter H. Anderson, Elmore, VT, July, '01

#case

#device PIC16HV540 *=8

#include <defs_540.h>
#include <delay.h>

#define TRUE !0
#define FALSE 0

void beep(long ms);
void beep_q(byte q, byte minus_flag);

void main(void)
{
   byte T_threshold = 34, T_C, minus_flag, n;
   byte const T_C_array[5] = {-5, 0, 1, 25, 70};  // some dummy temperatures

   DIRA = 0x0f;
   DIRB = 0xff;

   while(1)
   {
      if(!portb7) // if switch at ground
      {
         beep_q(T_threshold, FALSE);
         delay_ms(5000);
      }

      else
      {
         for (n = 0; n< 5; n++) // beep each temperature
         {
            T_C = T_C_array[n];
            if (T_C & 0x80)   // its negative
            {
               minus_flag = TRUE;
               T_C = (~T_C) + 1;
            }
            else
            {
               minus_flag = FALSE;
            }
            beep_q(T_C, minus_flag);
            delay_ms(1000);
        }
     }
     delay_ms(5000);
   }
}

void beep_q(byte q, byte minus_flag)
{
   byte n, digit;

   if (minus_flag)
   {
      beep(500);
      delay_ms(1000); // long delay to indicate minus
   }

   digit = q/10; // number of tens
   if (digit)     // if non zero
   {
      for (n=0; n<digit; n++)
      {
         beep(250);
         delay_ms(250);
      }

      delay_ms(1000); // separation between digits
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

   delay_ms(1000); // separation between digits
}

void beep(long ms)
{
   byte tmr0_old, tmr0_new;

   portb0 = 0;
   dirb0 = 0;
#asm
   MOVF DIRB, W
   TRIS PORTB
#endasm

   t0cs = 0;    // internal fosc / 4
   psa = 0;    // prescaler assigned to TMR0
   ps2 = 0;    // prescale of 1:4, thus rollover every ms
   ps1 = 0;
   ps0 = 1;

#asm
   MOVF OPTIONS1, W
   OPTION
#endasm
   TMR0 = 0x00;
   tmr0_old = 0;

   while(ms)
   {
      tmr0_new = TMR0;
      if ((tmr0_new < 0x80) && (tmr0_old >= 0x80)) // if there was a rollover
      {
         portb0 = !portb0;      // toggle speaker output
         --ms;
      }
      tmr0_old = tmr0_new;
   }
   portb0 = 0;
}

#include <delay.c>

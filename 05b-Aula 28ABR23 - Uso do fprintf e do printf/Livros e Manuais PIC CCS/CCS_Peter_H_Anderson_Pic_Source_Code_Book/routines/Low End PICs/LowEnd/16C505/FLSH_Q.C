// FLSH_Q.C (PIC16C505), CCS PCB
//
// Intended for possible use with frost alarm in place of serial output
// to serial LCD or to PC Com Port.
//
// When input PORTB3 is at ground, T_threshold is output to LED on PORTC0.
// When input PORTB3 is not at ground, the current value of T_C is output on
// the LED.  Note that readings are dummied in a constant array.
//
// In outputting the quantity, a long flash indicates a minus.  Each digit is
// output as a series of 250 ms flashes.  Interdigit time of 1 sec.  Five seconds
// between outputting each quantity.
//
//
//  GRD --- \---- PORTB3 (term 4) (internal weak pull-up enabled)
//
//                PORTC0 (term 10)  --------- 330 --->|--- GRD
//
// Use internal RC oscillator.   Internal /MCLR.  Tested using RICE-17, July 19, '01
//
// copyright, Peter H. Anderson, Elmore, VT, July, '01

#case

#device PIC16C505 *=8

#include <defs_505.h>
#include <delay.h>

#define TRUE !0
#define FALSE 0

void flash_q(byte q, byte minus_flag);

void main(void)
{
   byte T_threshold = 34, T_C, minus_flag, n;
   byte const T_C_array[5] = {-5, 0, 1, 25, 70};  // dummy values of temperature

   DIRB = 0x3f;
   DIRC = 0x3f;

   not_rbpu = 0; // enable internal weak pullups
#asm
    MOVF OPTIONS, W
    OPTION
#endasm

   while(1)
   {

     if(!portb3) // if switch at ground
     {
        flash_q(T_threshold, FALSE); // assumed to never be false
        delay_ms(5000); // five secs between displays
     }

     else
     {
        for (n = 0; n< 5; n++)  // for each of five temperatures
        {
           T_C = T_C_array[n];
           if (T_C & 0x80)  // if its negative
           {

              minus_flag = TRUE;
              T_C = (~T_C) + 1;
           }
           else
           {
              minus_flag = FALSE;
           }
           flash_q(T_C, minus_flag);
           delay_ms(5000);
        } // end of for
     } // end of else
  }  // end of while
}

void flash_q(byte q, byte minus_flag)
{
   byte n, digit;

   dirc0 = 0;
#asm
   MOVF DIRC, W
   TRIS PORTC
#endasm

   if (minus_flag)
   {
       portc0 = 1;
       delay_ms(500); // long delay to indicate minus
       portc0 = 0;
       delay_ms(1000); // delay between digits
   }

   digit = q/10; // number of tens
   if (digit)     // if non zero
   {
      for (n=0; n<digit; n++)
      {
         portc0 = 1;
         delay_ms(250);
         portc0 = 0;
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
       portc0 = 1;
       delay_ms(250);
       portc0 = 0;
       delay_ms(250);
   }

   delay_ms(500); // separation between digits
}

#include <delay.c>

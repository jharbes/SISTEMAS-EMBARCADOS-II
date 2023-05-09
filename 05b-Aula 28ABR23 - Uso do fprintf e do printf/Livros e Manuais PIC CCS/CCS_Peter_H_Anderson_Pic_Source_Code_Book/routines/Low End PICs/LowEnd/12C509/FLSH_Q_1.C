// FLSH_Q.C (PIC12C509), CCS PCB
//
// When input GP3 is at ground, T_threshold is output to Bicolor LED on GP5.
// When input GP3 is not at ground, the current value of T_C is output to
// the bicolor LED on GP4.  Note that readings are dummied in a constant array.
//
// In outputting the quantity, a long flash indicates a minus.  Each digit is
// output as a series of 250 ms flashes.  Interdigit time of 1 sec.  Five seconds
// between outputting each quantity.
//
//
//               PIC12C509
//
//  GRD ---- \-------------- GP3 (term 4)
//
//                              		 Bicolor LED
//             GP4 (term 3) ---------------->|---- |------ 330 ---- GRD
//             GP5 (term 2) ---------------->|---- |
//
// copyright, Peter H. Anderson, Elmore, VT, July, '01

#case

#device PIC12C509

#include <defs_509.h>
#include <delay.h>

#define TRUE !0
#define FALSE 0

#define LED_ALM   5		// use GP5 and GP4
#define LED_TEMP  4

void flash_q(byte LED, byte q, byte minus_flag);

void main(void)
{
   byte T_threshold = 34, T_C, minus_flag, n;
   char const T_C_array[5] = {-5, 0, 1, 25, 70};

   DIRS = 0x3f;

#asm
   BCF GPIO, LED_ALM    // make LED pins output logic zeros
   BCF GPIO, LED_TEMP
   BCF DIRS, LED_ALM
   BCF DIRS, LED_TEMP
   MOVF DIRS, W
   TRIS GPIO
#endasm

   not_gppu = 0;  // enable internal weak pull-ups
#asm
   MOVF OPTIONS, W
   OPTION
#endasm

   while(1)
   {

     if(!gp3) // if switch at ground
     {
        flash_q(LED_ALM, T_threshold, FALSE); // display the T_thresh on LED on GP5
        delay_ms(5000);
     }

    else
    {
        for (n = 0; n< 5; n++)
        {
           T_C = T_C_array[n];
           if (T_C & 0x80)    // if its negative
           {
              minus_flag = TRUE;
              T_C = (~T_C) + 1;   // 2's comp
           }
           else
           {
              minus_flag = FALSE;
           }
           flash_q(LED_TEMP, T_C, minus_flag); // display on the LED on GP4
        }
     }
     delay_ms(5000);
  }
}

void flash_q(byte LED, byte q, byte minus_flag)
{
   byte n, digit;

#asm
   BCF GPIO, LED_ALM    // make LED pins output logic zeros
   BCF GPIO, LED_TEMP
   BCF DIRS, LED_ALM
   BCF DIRS, LED_TEMP
   MOVF DIRS, W
   TRIS GPIO
#endasm

   if (minus_flag)
   {
       if (LED == LED_ALM)
       {
#asm
        BSF GPIO, LED_ALM
#endasm
       }
       else
       {
#asm
          BSF GPIO, LED_TEMP
#endasm
       }

      delay_ms(500); // long delay to indicate minus
#asm
      BCF GPIO, LED_ALM
      BCF GPIO, LED_TEMP
#endasm
      delay_ms(1000);
   }

   digit = q/10; // number of tens
   if (digit)     // if non zero
   {
      for (n=0; n<digit; n++)
      {

         if (LED == LED_ALM)
         {
#asm
            BSF GPIO, LED_ALM
#endasm
         }
         else
         {
#asm
            BSF GPIO, LED_TEMP
#endasm
         }
         delay_ms(250); // long delay to indicate minus
#asm
         BCF GPIO, LED_ALM
         BCF GPIO, LED_TEMP
#endasm
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
      if (LED == LED_ALM)
      {
#asm
         BSF GPIO, LED_ALM
#endasm
      }
      else
      {
#asm
          BSF GPIO, LED_TEMP
#endasm
      }
      delay_ms(250); // long delay to indicate minus
#asm
      BCF GPIO, LED_ALM
      BCF GPIO, LED_TEMP
#endasm
      delay_ms(250);
   }

   delay_ms(1000); // separation between digits
}

#include <delay.c>

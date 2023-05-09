// FLSH_Q.C (PIC16HV540), CCS PCB
//
// When input PORTB7 is at ground, T_threshold is output to LED on PORTA0.
// When input PORTB7 is not at ground, the current value of T_C is output on
// the LED.  Note that readings are dummied in a constant array.
//
// In outputting the quantity, a long flash indicates a minus.  Each digit is
// output as a series of 250 ms flashes.  Interdigit time of 1 sec.  Five seconds
// between outputting each quantity.
//
//    			+12 VDC
//    				|
//    			 	10K  	PIC16HV540
//     				|
//  GRD ---- \-------------- PORTB7 (term 13)			LED
//							 PORTA0 -------------- 330 -->|---- GRD
// 4.0 MHz Crystal, WDT Off
//
// copyright, Peter H. Anderson, Elmore, VT, July, '01

#case

#device PIC16HV540 *=8

#include <defs_540.h>
#include <delay.h>

#define TRUE !0
#define FALSE 0

void config_processor(void);
void flash_q(byte q, byte minus_flag);

void main(void)
{
   byte T_threshold = 34, T_C, minus_flag, n;
   char const T_C_array[5] = {-5, 0, 1, 25, 70};

   DIRA = 0x0f;
   DIRB = 0xff;

   config_processor();	// set for 5V logic on PortA

   while(1)
   {

     if(!portb7) // if switch at ground
     {
        flash_q(T_threshold, FALSE);	// display the T_thresh
        delay_ms(5000);
     }

    else
    {
        for (n = 0; n< 5; n++)
        {
           T_C = T_C_array[n];
           if (T_C & 0x80)				// if its negative
           {
              minus_flag = TRUE;
              T_C = (~T_C) + 1;			// 2's comp
           }
           else
           {
              minus_flag = FALSE;
           }
           flash_q(T_C, minus_flag);
        }
     }
     delay_ms(5000);
  }
}

void flash_q(byte q, byte minus_flag)
{
   byte n, digit;

   dira0 = 0;
#asm
   MOVF DIRA, W
   TRIS PORTA
#endasm

   if (minus_flag)
   {
      porta0 = 1;
      delay_ms(500); // long delay to indicate minus
      porta0 = 0;
      delay_ms(1000);
   }

   digit = q/10; // number of tens
   if (digit)     // if non zero
   {
      for (n=0; n<digit; n++)
      {
         porta0 = 1;
         delay_ms(250);
         porta0 = 0;
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
       porta0 = 1;
       delay_ms(250);
       porta0 = 0;
       delay_ms(250);
   }

   delay_ms(1000); // separation between digits
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

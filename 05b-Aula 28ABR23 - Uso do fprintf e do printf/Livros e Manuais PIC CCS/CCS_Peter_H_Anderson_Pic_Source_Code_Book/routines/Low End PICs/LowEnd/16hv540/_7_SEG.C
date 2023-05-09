// _7_SEG_1.C (PIC16HV540), CCS PCB
//
// PIC16HV540       Common Anode 7-Seg LED
//
// PORTB6 (term 12) ----- 1K ----------- a seg (term 1)   ----------- +12 VDC (term 3, 14)
// PORTB5 (term 11) ----- 1K ----------- b seg (term 13)
// PORTB4 (term 10) ----- 1K ----------- c seg (term 10)
// PORTB3 (term 9) ------ 1K ----------- d seg (term 8)
// PORTB2 (term 8) ------ 1K ----------- e seg (term 7)
// PORTB1 (term 7) ------ 1K ----------- f seg (term 2)
// PORTB0 (term 6) ------ 1K ----------- g seg (term 11)
//
//     +12 VDC
//         |
//      10K
//      |
//  GRD -------- \------------ PORTB7 (term 13)
//
//          a   // Layout of 7-seg LED
//        f   b
//          g
//        e   c
//          d
//
// copyright, Peter H. Anderson, Elmore, VT, July, '01

#case

#device PIC16HV540

#include <defs_540.h>
#include <delay.h>

#define TRUE !0
#define FALSE 0

#define MINUS_SIGN 0x01 // segment g only
#define BLANK 0x00  // no segments

void config_processor(void);
void display_q(byte q, byte minus_flag);
void display_patt(byte patt);

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
         display_q(T_threshold, FALSE);
         delay_ms(2000);
      }

      else
      {
         for (n = 0; n< 5; n++)
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
            display_q(T_C, minus_flag);
            delay_ms(2000);
       }
    }
    delay_ms(5000); // repeat time
   }
}

void display_q(byte q, byte minus_flag)
{
   byte const hex_digit_patts[16]=
   //    0     1     2     3     4     5     6     7
      {0x7e, 0x30, 0x6d, 0x79, 0x33, 0x5b, 0x5f, 0x70,
   //    8     9     A     B     C     D     E     F
       0x7f, 0x7b, 0x77, 0x1f, 0x4e, 0x3d, 0x4f, 0x47};

   byte const help_patts[4] = {0x37, 0x4f, 0x0f, 0x67}; // info only
                                //  H     E     L     P
   byte n, digit;

   if (minus_flag)
   {
      display_patt(MINUS_SIGN);
      delay_ms(500);
      display_patt(BLANK);
      delay_ms(500);
   }

   digit = q/10; // number of tens

   display_patt(hex_digit_patts[digit]);
   delay_ms(500);
   display_patt(BLANK);
   delay_ms(500);

   digit = q%10;

   display_patt(hex_digit_patts[digit]);
   delay_ms(500);
   display_patt(BLANK);
   delay_ms(500);
}

void display_patt(byte patt)
{

   patt = (~patt) & 0x7f; // convert to negative logic

   DIRB = DIRB & 0x80;  // make lowest seven bits outputs

#asm
    MOVF DIRB, W
    TRIS PORTB
#endasm

    PORTB = (PORTB & 0x80) | patt;
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

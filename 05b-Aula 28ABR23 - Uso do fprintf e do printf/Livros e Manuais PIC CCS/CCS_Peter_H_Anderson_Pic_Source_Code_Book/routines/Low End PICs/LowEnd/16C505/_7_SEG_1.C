// _7_SEG_1.C (PIC16HV540), CCS PCB
//
// PIC16C505           Common Anode 7-Seg LED (MAN72A or LSD3221-11, Jameco #24740)
//
// PORTB5 (term 2) ------ 330 ----------- a seg (term 1) ----|< ------------- +5 VDC (terms 3, 14)
// PORTB4 (term 3) ------ 330 ----------- b seg (term 13)
//
// PORTC4 (term 6) ------ 330 ----------- c seg (term 10)
// PORTC3 (term 7) ------ 330 ----------- d seg (term 8)
// PORTC2 (term 8) ------ 330 ----------- e seg (term 7)
// PORTC1 (term 9) ------ 330 ----------- f seg (term 2)
// PORTC0 (term 10) ----- 330 ----------- g seg (term 11)
//
//          a   // Layout of 7-seg LED
//        f   b
//          g
//        e   c
//          d
//
// copyright, Peter H. Anderson, Baltimore, MD, July, '01

#case

#device PIC16C505 *=8

#include <defs_505.h>
#include <delay.h>

#define TRUE !0
#define FALSE 0

#define MINUS_SIGN 0x01 // segment g only
#define BLANK 0x00  // no segments

void display_q(byte q, byte minus_flag);
void display_patt(byte patt);

void main(void)
{
    byte T_threshold = 34, T_C, minus_flag, n;
    char const T_C_array[5] = {-5, 0, 1, 25, 70};

    DIRC = 0x3f;
    DIRB = 0x3f;

    not_rbpu = 0; // enable weak pullups on PORTB
#asm
    MOVF OPTIONS, W
    OPTION
#endasm

    while(1)
    {

       if(!portb3) // if switch at ground
       {
          display_q(T_threshold, FALSE);
          delay_ms(2000);
       }

       else
       {
          for (n = 0; n< 5; n++)
          {
             T_C = T_C_array[n];
             if (T_C & 0x80)
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
   byte segs_low_five, segs_high_two;

   patt = ~patt; // convert to negative logic
   segs_low_five = patt & 0x1f;
   segs_high_two = (patt >> 5) & 0x03;

   DIRC = DIRC & 0x20;  // make lowest five bits outputs
   DIRB = DIRB & 0x0f;       // make bits 5 and 4 outputs
#asm
   MOVF DIRC, W
   TRIS PORTC
   MOVF DIRB, W
   TRIS PORTB
#endasm

   PORTC = (PORTC & 0x20) | segs_low_five; // output the low five bits
   PORTB = (PORTB & 0x0f) | (segs_high_two << 4);
}

#include <delay.c>

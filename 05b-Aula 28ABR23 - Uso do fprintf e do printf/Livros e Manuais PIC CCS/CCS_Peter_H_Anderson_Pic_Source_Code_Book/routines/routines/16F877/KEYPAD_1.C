// KeyPad_1.C (PIC16F877)
//
// Illustrates an interface with a 4X4 key pad.  Program continually
// loops, checking if key_present and is so displays the key_character
// assigned to the depressed key.
//
// Nothe that the reading of the key is performed in the interrupt service
// routine.
//
// PIC16F877	  KeyPad
//
// RB7 <--------- Row 3					1 2 3 A
// RB6 <--------- Row 2					4 5 6 B
// RB5 <--------- Row 1					7 8 9 C
// RB4 <--------- Row 0					* 0 # D
//										Key Pad Layout
// RB3 ---------> Col 3
// RB2 ---------> Col 2
// RB1 ---------> Col 1
// RB0 ---------> Col 0
//
// copyright, Peter H. Anderson, Baltimore, MD, May, '01

#case

#device PIC16F877 *=16 ICD=TRUE

#include <defs_877.h>
#include <lcd_out.h> // LCD and delay routines

#define TRUE !0
#define FALSE 0

// #define _2_ROW // See Text

byte get_key(byte *p_row, byte *p_col);
void debounce(byte ms);		// separte delay routine to avoid recursion

byte key_present, key;

void main(void)
{
   byte const key_char[16] = { '1', '2', '3', 'A', '4', '5', '6', 'B',
                               '7', '8', '9', 'C', '*', '0', '#', 'D'};
   lcd_init();
   not_rbpu = 0;
   pspmode = 0;
   TRISB = 0xf0;	// high nibble are row inputs, low are col outputs
   PORTB = 0x00;

   key_present = FALSE;

   rbif = 0;
   rbie = 1;
   gie = 1;

   while(1)
   {
      if (key_present)
      {
         while(gie)
         {
            gie = 0;
         }

         lcd_char(key_char[key]);
         key_present = FALSE;
         rbif = 0;	// not really necessary
         rbie = 1;	// ''
         gie = 1;
      }
   } // of while 1
}

void debounce(byte ms)		// note that a separate delay function was used to
{							// avoid inadvertent recursion.
   byte t;
   do
   {
    t = 100;				// about 100 * 10 us
#asm
      BCF STATUS, RP0
DELAY_10US_1:
      CLRWDT
      NOP
      NOP
      NOP
      NOP
      NOP
      NOP
      DECFSZ t, F
      GOTO DELAY_10US_1
#endasm

   } while(--ms);
}

byte get_key(byte *p_row, byte *p_col)
{
   byte row, col, in_patt;
   byte const patt[4] = {0x0e, 0x0d, 0x0b, 0x07};

   for (col = 0; col < 4; col++)
   {
      TRISB = (TRISB & 0xf0) | patt[col];
#ifdef _2_ROW
      in_patt = (PORTB >> 4) & 0x03;
      if(in_patt != 0x03)     // both high
#else
      in_patt = (PORTB >> 4) & 0x0f;
      if(in_patt != 0x0f)     // all high
#endif
      {
         for (row = 0; row < 4; row++)
         {
#ifdef _2_ROW
            if (in_patt == (patt[row] & 0x03))
#else
            if (in_patt == (patt[row] & 0x0f))
#endif
            {
               *p_row = row;
               *p_col = col;
               TRISB = TRISB & 0xf0;	// restore ground to all col outputs
               return(TRUE);
            }
         }
      }
   }
   TRISB = TRISB & 0xf0;
   return(FALSE);
}

#int_rb rb_int_handler(void)
{
   byte row, col;
   debounce(50);
   if (get_key(&row, &col) == TRUE)
   {
      key_present = TRUE;
      key = 4 * row + col;
   }
}

#int_default default_interrupt_handler()
{
}

#include <lcd_out.c>

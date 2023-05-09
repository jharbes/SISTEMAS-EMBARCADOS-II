// KeyPad_4.C (PIC16F628)
//
// Illustrates an interface with a 4X4 key pad with a 20 byte buffer.  Program
// continually loops. On interrupt on change, the interrupt service routine adds
// a valid key to a 20 byte circular buffer.  On receipt of a serial character from
// a master processor, the keypad decoder PIC sends the content of the key buffer to
// the master at 9600 baud.
//
//
// 					PIC16F877   KeyPad
//
// 					RB7 <--------- Row 3     1 2 3 A
// 					RB6 <--------- Row 2     4 5 6 B
// 					RB5 <--------- Row 1     7 8 9 C
// 					RB4 <--------- Row 0     * 0 # D
// 									         Key Pad Layout
// 					RA3 ---------> Col 3
// 					RA2 ---------> Col 2
// 					RA1 ---------> Col 1
// 					RA0 ---------> Col 0
//
// BX24
//
//	   -------->  RX/RB1
//	   <--------  TX/RB2
//
// copyright, Peter H. Anderson, Baltimore, MD, May, '01

#case

#device PIC16F628 *=16

#include <defs_628.h>
#include <ser_628.h> // serial routines
#include <delay.h>

#define TRUE !0
#define FALSE 0

byte get_key(byte *p_row, byte *p_col);
void debounce(byte ms);  // separate delay routine to avoid recursion

byte keys[20], get_index, put_index, rda_int_occ;

void main(void)
{
   byte const key_char[16] = { '1', '2', '3', 'A', '4', '5', '6', 'B',
                               '7', '8', '9', 'C', '*', '0', '#', 'D'};
   byte key, ch;

   CMCON = 0x07;  // comparators off
   vroe = 0;   // voltage ref disabled on RA2

   asynch_enable();
   not_rbpu = 0;

   TRISB = 0xff; // high nibble are row inputs, low nibble is TX and RX
   PORTA = 0x00;
   TRISA = 0xf0; // column outputs

   put_index = 0;
   get_index = 0;

   ch = RCREG;  // get any junk that may be in the buffer
   ch = RCREG;

   rda_int_occ = FALSE;

   rcif = 0;  // receive data interrupt
   rcie = 1;
   peie = 1;

   rbif = 0;
   rbie = 1;

   gie = 1;

   while(1)
   {
      rcie = 1;
      peie = 1;
      rbie = 1;
      gie = 1;
#asm
      CLRWDT
#endasm
      if (rda_int_occ)
      {
		 delay_ms(10);		// a bit of a delay for if the interface is a Basic Stamp
         while (get_index != put_index)
         {
            key = keys[get_index];
            ser_char(key_char[key]);
            ++get_index;
            if (get_index > 19)
            {
               get_index = 0;
            }
         }
         ser_char(13);  // terminate the string with new line
         ser_char(10);
         rda_int_occ = FALSE;
     }
   } // of while 1
}

void debounce(byte ms)  // note that a separate delay function was used to
{       				// avoid inadvertent recursion.
   byte t;
   do
   {
    t = 100;    // about 100 * 10 us
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
      TRISA = (TRISA & 0xf0) | patt[col];
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
               TRISA = TRISA & 0xf0; // restore ground to all col outputs
               return(TRUE);
            }
         }
      }
   }
   TRISA = TRISA & 0xf0;
   return(FALSE);
}

#int_rb rb_int_handler(void)
{
   byte row, col;
   debounce(50);
   if (get_key(&row, &col) == TRUE)
   {
      keys[put_index] = 4 * row + col;
      ++put_index;
      if (put_index > 19)
      {
        put_index = 0;
      }
   }
}

#int_rda rda_interrupt_handler(void)
{
   byte ch;
   rda_int_occ = TRUE;
   ch = RCREG;
}

#int_default default_interrupt_handler()
{
}

#include <ser_628.c>
#include <delay.c>

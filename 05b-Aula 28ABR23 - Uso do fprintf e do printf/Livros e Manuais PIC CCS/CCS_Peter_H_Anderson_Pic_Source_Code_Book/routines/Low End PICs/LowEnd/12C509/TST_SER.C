// TST_SER.C  (PIC12C509), CCS PCB
//
//
// Illustrates the use of various serial output functions contained in "ser_509.c".
//
// PIC12C509
//
//  GP0 (term 7) ------------------------ Serial LCD or PC Com Port
//
// copyright, Peter H. Anderson, Elmore, VT, July, '01

#case

#device PIC12C509 *=8

#include <defs_509.h>
#include <string.h> // for strcpy

#include <delay.h>
#include <ser_509.h>

#define TxData 0 // use GP0
#define INV // send inverted RS232

void main(void)
{
   byte s[8], n;
   n = 150;

   while(1)
   {
      DIRS=0x3f;

      ser_init();
      strcpy(s, "Morgan");
         // note that CONST string is copied to RAM string
      ser_out_str(s);
      strcpy(s, " State");
      ser_out_str(s);
      ser_new_line();
      strcpy(s, "Univer");
      ser_out_str(s);
      strcpy(s, "sity");
      ser_out_str(s);
      ser_new_line();
      ser_hex_byte(n);
      ser_char(' ');
      ser_dec_byte(n, 3); // display in dec, three places

      delay_ms(500);
      ++n;
   }
}

#include <delay.c>
#include <ser_509.c>

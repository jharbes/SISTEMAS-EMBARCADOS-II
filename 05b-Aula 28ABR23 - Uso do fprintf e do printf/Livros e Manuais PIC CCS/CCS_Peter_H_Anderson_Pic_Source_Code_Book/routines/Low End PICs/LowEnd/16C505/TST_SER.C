// TST_SER.C  (PIC16C505), CCS PCB
//
// Illustrates the use of various serial output functions contained in "ser_505.c".
//
// PIC16C505
//
//  PORTC0 (term 10) ------------------------ Serial LCD or PC Com Port
//
// copyright, Peter H. Anderson, Elmore, VT, July, '01

#case 
#device PIC16C505 *=8

#include <defs_505.h>
#include <string.h> // for strcpy
#include <ser_505.h>
#include <delay.h>

#define TxData 0
#define INV // send inverted RS232

void main(void)
{
   byte i, n;
   byte const str1[20] = {"   Morgan State\0"};
   byte const str2[20] = {"    University\0"};
   n = 150;

   DIRB=0x3f;
   DIRC = 0x3f;

   while(1)
   {
      ser_init();

      i = 0;
      while(str1[i])
      {
         ser_char(str1[i]);
         ++i;
      }

      ser_new_line();

      i = 0;
      while(str2[i])
      {
         ser_char(str2[i]);
         ++i;
      }

      ser_new_line();

      ser_hex_byte(n);  // display in hex format
      ser_char(' ');
      ser_dec_byte(n, 3); // display in dec, three places

      delay_ms(500);
      ++n;
   }
}

#include <delay.c>
#include <ser_505.c>

// TST_SER.C  (PIC12C672), CCS PCB
//
// Illustrates the use of various serial output functions contained in "ser_672.c".
//
// PIC12C672
//
//  GP1 (term 6) ------------------------ Serial LCD or PC Com Port
//
// copyright, Peter H. Anderson, Elmore, VT, July, '01

#case

#device PIC12C672

#include <defs_672.h>
#include <string.h> // for strcpy

#include <delay.h>
#include <ser_672.h>

#define TxData 1 // use GP1
#define INV // send inverted RS232

#define TRUE !0
#define FALSE 0

void calibrate(void);

void main(void)
{
   byte s[20], n;
   n = 150;

   pcfg2 = 1; // configure A/D for AN0 (GP0) - Not used in this example
   pcfg1 = 1; // others as IO
   pcfg0 = 0;

   calibrate();  // no calibrate when using emulator
   while(1)
   {
      ser_init();
      strcpy(s, "Morgan State");
         // note that CONST string is copied to RAM string
      ser_out_str(s);
      ser_new_line();
      strcpy(s, "University");
      ser_out_str(s);

      ser_new_line();
      ser_hex_byte(n);
      ser_char(' ');
      ser_dec_byte(n, 3); // display in dec, three places

      delay_ms(500);
      ++n;
   }
}

void calibrate(void)
{
#asm
   CALL 0x03ff		// 0x01ff for 12C671 and CE673
   MOVWF OSCCAL
#endasm
}

#include <delay.c>
#include <ser_672.c>

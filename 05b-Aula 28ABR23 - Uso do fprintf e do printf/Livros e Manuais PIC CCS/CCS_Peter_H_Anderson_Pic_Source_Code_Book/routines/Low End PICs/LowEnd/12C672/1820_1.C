// 1820_1.C  PIC12C672, CCS PCM
//
// Measures temperature using a Dallas DS1820 on GP2 and displays result
// using RS232 serial (9600 inverted) on GP1.

// PIC12C672
//    					 +5VDC
//    					  |
//    					 4.7K
//    					  |
//  GP2 (term 5) --------------------------- DQ of DS18S20
//  GP1 (term 6) --------------------------- To Ser LCD or PC Com Port
//
// Debugged using RICE-17A Emulator, July 19, '01.
//
// copyright, Peter H. Anderson, Elmore. VT, July, '01

#case

#device PIC12C672

#include <defs_672.h>

#include <delay.h>
#include <ser_672.h>
#include <_1_wire.h>

#define TxData 1 // use GP1
#define INV // send inverted RS232

#define TRUE !0
#define FALSE 0

#define _1W_PIN 2

void calibrate(void);

void main(void)
{
   byte T_C, sign;

//   calibrate();		// do not use this function during emulation

   pcfg2 = 1; // configure A/D for AN0 (GP0) - Not used in this example
   pcfg1 = 1; // others as IO
   pcfg0 = 0;

   while(1)
   {
      ser_init();
      _1w_init(_1W_PIN);
      _1w_out_byte(_1W_PIN, 0xcc);  // skip ROM
      _1w_out_byte(_1W_PIN, 0x44);  // perform temperature conversion

      _1w_strong_pull_up(_1W_PIN);

      _1w_init(_1W_PIN);
      _1w_out_byte(_1W_PIN, 0xcc);  // skip ROM
      _1w_out_byte(_1W_PIN, 0xbe);

      T_C = _1w_in_byte(_1W_PIN);
      sign = _1w_in_byte(_1W_PIN);

      if (sign)   // negative
      {
         T_C = ~T_C + 1;
         ser_char('-');
      }

      T_C = T_C / 2;

      if (T_C > 99) // unlikely
      {
          ser_dec_byte(T_C, 3);
      }
      else if (T_C > 9)
      {
          ser_dec_byte(T_C, 2);
      }
      else
      {
          ser_dec_byte(T_C, 1);
      }
      delay_ms(1000);
   }
}

void calibrate(void)
{
#asm
   CALL 0x03ff		// 0x3fff for 12C672/CE674, 0x1fff for 12C671/CE673
   MOVWF OSCCAL
#endasm
}

#include <delay.c>
#include <ser_672.c>
#include <_1_wire.c>


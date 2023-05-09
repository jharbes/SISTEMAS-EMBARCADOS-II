// TST_SER.C  (PIC16HV540), CCS PCB
//
// copyright, Peter H. Anderson, Elmore, VT, July, '01

#case

#device PIC16HV540

#include <defs_540.h>
#include <string.h> // for strcpy

#include <delay.h>
#include <ser_540.h>

#define TxData 0 // use PORTA0
#define INV // send inverted RS232

void config_processor(void);

void main(void)
{
   byte i, n;
   byte const str1[20] = {"   Morgan State\0"};
   byte const str2[20] = {"    University\0"};
   n = 150;

   DIRA = 0x0f;
   DIRB = 0xff;
   config_processor();

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
#include <ser_540.c>

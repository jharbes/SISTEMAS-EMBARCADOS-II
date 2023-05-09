// FLASH_2.C (PIC12C509)  CCS PCB
//
// This is a simple rework of FLASH_1.C where functions flash(), delay_ms() and delay_10us()
// are all called twice.  The intent was to investigate how the compiler handles the two
// level stack limitation.
//
// Flashes an LED on GP1 in bursts of five flashes if input on GP3 is at logic zero.
//
// Note that DIRS and OPTIONS are  defined in defs_509.h
//
//
//  GRD ---------- \-------- GP3 (term 4) (weak pullups enabled)
//                           GP1 (term 6) ------------ 330 ------->|------- GRD
//
//
// copyright, Peter H. Anderson, Elmore, VT, July, '01

#case

#device PIC12C509 *=8

#include <defs_509.h>

void flash(byte num_flashes);
void delay_10us(byte t);
void delay_ms(long t);

void main(void)
{
   DIRS = 0x3f;
   dirs1 = 0;      // make gp1 an output
#asm
   MOVF DIRS, W
   TRIS GPIO
#endasm

   not_gppu = 0;
#asm
   MOVF OPTIONS, W
   OPTION
#endasm

   while(1)
   {
     while(gp3)
     {
     }   // loop until at logic zero
     flash(3);			// <<<<<<<<<<<<<<<<< note that flash() is called twice
     flash(2);
     delay_ms(3000);
   }
}

void flash(byte num_flashes)
{
   byte n;
   for (n=0; n<num_flashes; n++)
   {
     gp1 = 1;
     delay_ms(500);		// <<<<<<<<<<<<<<<< note that delay_ms() is called twice
     gp1 = 0;
     delay_ms(500);
   }
}

void delay_10us(byte t)
{
#asm
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
}

void delay_ms(long t)   // delays t millisecs
{
   do
   {
     delay_10us(50);		// <<<<<<<<<<<<< note that delay_10us() is called twice
     delay_10us(50);
   } while(--t);
}




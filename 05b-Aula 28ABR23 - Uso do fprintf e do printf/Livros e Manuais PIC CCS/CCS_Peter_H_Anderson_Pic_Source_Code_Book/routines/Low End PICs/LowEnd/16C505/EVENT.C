// EVENT.C (PIC16C505), CCS-PCB
//
// Normally, an LED is on.  If there was one or more events on T0CKI, the
// LED is flashed 10 times at 10 pulses per second.
//
//           +5 VDC
//            |
//            10K
//             |
//  GRD ----------- \-------------- T0CKI/PORTC5 (term 5)
//                                  PORTC4 (term 6) ----------- 330 ----->| ----- GRD
//
//
// Tested using RICE-17A on July 22, '01
//
// copyright, Peter H. Anderson, Baltimore, MD, July, '01

#case

#device PIC16C505 *=8

#include <defs_505.h>
#include <delay.h>

void main(void)
{
    byte n;

    DIRB = 0x3f;
    DIRC = 0x3f;

    t0cs = 1; // external event
    psa = 1;  // prescale assigned to WDT
    ps2 = 0;  // 1:1 prescale
    ps1 = 0;
    ps0 = 0;

#asm
   MOVF OPTIONS, W
   OPTION
#endasm
    TMR0 = 0x00;

    while(1)
    {
       portc4 = 1;
       dirc4 = 0;
#asm
       MOVF DIRC, W
       TRIS PORTC
#endasm

       if (TMR0)
       {
          TMR0 = 0x00;
          for (n = 0; n<10; n++)
          {
            portc4 = 0;
            delay_ms(50);
            portc4 = 1;
            delay_ms(50);
          }
       }
   }
}

#include <delay.c>


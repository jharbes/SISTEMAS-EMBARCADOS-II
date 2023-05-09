// RCTIME.C (PIC12C509), CCS-PCB
//
// Charges capacitor in parallel with a resistor on GP2 for one second.
// GP2 is then made an input and capacitor discharges though capacitor and
// and the time for detection of a one to zero transition is measured.
//
// Result in number of 1 usec ticks is displayed in hex on serial LCD
// on GP0.
//
// Illustrates use of TMR0
//
//.
//   GP2 (term 5) --- 330 ----- --------
//                               |        |
//                             1.0 uFd    10K Pot
//                               |        |
//                               |        10K Resistor
//                               |        |
//                              GRD       GRD
//
//   GP0 (term 7) ---------------- To Serial LCD or PC COM Port
//
// copyright, Peter H. Anderson, Elmore, VT, July, '01

#case

#device PIC12C509 // *=8

#include <defs_509.h>
#include <delay.h>
#include <ser_509.h>

#define TxData 0
#define INV

void main(void)
{
    byte count_hi, count_lo, tmr0_old, tmr0_new;

    DIRS = 0x3f;
    ser_init();

    while(1)
    {

       t0cs = 0; // fosc / 4 is clock source
       psa = 1;  // prescale assigned to WDT
       ps2 = 0;  // 1:1 prescale
       ps1 = 0;
       ps0 = 0;

#asm
       MOVF OPTIONS, W
       OPTION
#endasm

       dir2 = 0;   // output
#asm
       MOVF DIRS, W
       TRIS GPIO
#endasm
       gp2 = 1; // charge capacitor

       delay_ms(10);

       count_hi = 0;
       count_lo = 0;
       tmr0_old = 0x00;
       TMR0 = 0x00;
#asm
       BSF DIRS, 2
       MOVF DIRS, W
       TRIS GPIO
#endasm
       while(1) // wait for cap to discharge
       {
          tmr0_new = TMR0;
          if ((tmr0_new < 0x80) && (tmr0_old >= 0x80))  // there was a roll over
          {
             ++count_hi;
             if (count_hi == 0) // no zero crossing with 65 ms
             {
                count_hi = 0xff;
                count_lo = 0xff;
                break;
             }

          }
          if (!gp2) // if capacitor discharged below zero corssing
          {
              count_lo = tmr0_new;
              break;
          }
          tmr0_old = tmr0_new;
       }
       ser_init();
       ser_hex_byte(count_hi);
       ser_hex_byte(count_lo);
       delay_ms(1000);
    }
}

#include <delay.c>
#include <ser_509.c>

// RCTIME.C (PIC16HV540), CCS-PCB
//
// Charges capacitor in parallel with a resistor on PORTA2 for 10 ms.
// PORTA2 is then made an input and capacitor discharges though capacitor and
// and the time for detection of a one to zero transition is measured.
//
// Result in number of 1 usec ticks is displayed in hex on serial LCD
// on PORTA0.
//
// Illustrates use of TMR0.
//
//  PIC16HV540
//   PORTA2 (term 1) --- 330 ----- --------
//                               |        |
//                             1.0 uFd    10K Pot
//                               |        |
//                               |        10K Resistor
//                               |        |
//                              GRD       GRD
//
//   PORTA0 (term 17) ---------------- To Serial LCD or PC COM Port
//
//
//
// copyright, Peter H. Anderson, Elmore, VT, July, '01

#case

#device PIC16HV540

#include <defs_540.h>
#include <delay.h>
#include <ser_540.h>

#define TxData 0 // use PORTA0
#define INV // send inverted RS232

void config_processor(void);

void main(void)
{
    byte count_hi, count_lo, tmr0_new, tmr0_old;

    config_processor();
    DIRA = 0x3f;
    while(1)
    {

       ser_init();

       t0cs = 0; // fosc / 4 is clock source
       psa = 1;  // prescale assigned to WDT
       ps2 = 0;  // 1:1 prescale
       ps1 = 0;
       ps0 = 0;

#asm
   MOVF OPTIONS1, W
   OPTION
#endasm

       dira2 = 0;   // output
#asm
   MOVF DIRA, W
   TRIS PORTA
#endasm
       porta2 = 1; // charge capacitor

       delay_ms(10);

       count_hi = 0;
       count_lo = 0;
       tmr0_old = 0x00;

       TMR0 = 0x00;
#asm
    BSF DIRA, 2
    MOVF DIRA, W
    TRIS PORTA
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
          if (!porta2) // if capacitor discharged below zero corssing
          {
              count_lo = tmr0_new;
              break;
          }
          tmr0_old = tmr0_new;
       }
       ser_hex_byte(count_hi);
       ser_hex_byte(count_lo);
       delay_ms(1000);
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



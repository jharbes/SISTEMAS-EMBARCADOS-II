// COUNT.C (PIC16C505), CCS-PCB
//
// Counts the number of pulses on input T0CKI over a period of time and displays events per
// minute on serial LCD or PC Com Port, 9600 baud, inverted.
//
//           	+5 VDC
//           	 |
//           	 10K
//           	 |
//  100 PPS ----------------------- T0CKI/PORTC5 (term 5)
//                                  PORTC0 (term 10) ---------------- To Serial LCD or PC Com Port
//
//
// Tested using RICE-17A on July 23, '01
//
// copyright, Peter H. Anderson, Baltimore, MD, July, '01

#case

#device PIC16C505 *=8

#include <defs_505.h>
#include <delay.h>
#include <ser_505.h>

#define TxData 0
#define INV


long count(long ms);

void main(void)
{
   long events, RPM;
   byte hi, lo;

    DIRB = 0x3f;
    DIRC = 0x3f;
    ser_init();

    while(1)
    {
       events = count(5000);
       RPM = events * (60 / 5);		  // 60 seconds / 5 sec sample time
       hi = (byte) (RPM / 100);		  // split into high and low bytes
       lo = (byte) (RPM % 100);

       ser_init();

       ser_dec_byte(hi, 3);
       ser_dec_byte(lo, 2);
    }
}

long count(long ms)
{
   byte ms_100, ms_1, count_hi, count_lo, tmr0_old, tmr0_new, n;
   long cnt;

   t0cs = 1; // external clock source
   psa = 1;  // prescale assigned to WDT
   ps2 = 0;  // 1:1 prescale
   ps1 = 0;
   ps0 = 0;
#asm
   MOVF OPTIONS, W
   OPTION
#endasm
   ms_100 = (byte) (ms / 100);
   ms_1 = (byte) (ms % 100);

   tmr0_old = 0;
   count_hi = 0;
   count_lo = 0;

   TMR0 = 0x00;
   for(n = 0; n < ms_100; n++)
   {
      delay_ms(100);
      tmr0_new = TMR0;

      if ((tmr0_new < 0x80) && (tmr0_old >= 0x80))  // there was a roll over
      {
          ++count_hi;
      }
      tmr0_old = tmr0_new;
   }

   if (ms_1)
   {
       delay_ms(ms_1);
       tmr0_new = TMR0;
       if ((tmr0_new < 0x80) && (tmr0_old >= 0x80))  // there was a roll over
       {
         ++count_hi;
       }
   }
   count_lo = tmr0_new;
   cnt = count_hi;
   cnt = (cnt << 8) | count_lo;
   return(cnt);
}

#include <delay.c>
#include <ser_505.c>

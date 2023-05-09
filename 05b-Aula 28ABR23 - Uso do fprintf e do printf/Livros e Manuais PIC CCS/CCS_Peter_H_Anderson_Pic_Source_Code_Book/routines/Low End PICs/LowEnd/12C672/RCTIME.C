// RCTIME.C (PIC12C672), CCS-PCM
//
// Charges capacitor in parallel with a resistor on GP2/INT for one second.
// PORTC1 is then made an input and capacitor discharges though capacitor and
// and the time for detection of a one to zero transition is measured.
//
// Result in number of 1 usec ticks is displayed in hex on serial LCD
// on GP0.
//
// Illustrates use of TMR0 and external interrupt.
//
//   GP2/INT (term 5) --- 330 ----- --------
//                                 |        |
//                               1.0 uFd    10K Pot
//                                 |        |
//                                 |        10K Resistor
//                                 |        |
//                                GRD       GRD
//
//   GP0 (term 7) ---------------- To Serial LCD or PC COM Port
//
// Tested using RICE-17A on July 19, '01
//
// copyright, Peter H. Anderson, Elmore, VT, July, '01

#case

#device PIC12C672 *=8

#include <defs_672.h>
#include <delay.h>
#include <ser_672.h>

#define TRUE !0
#define FALSE 0

#define TxData 0
#define INV

byte ext_int_occ;
byte count_hi, count_lo; //note global

void main(void)
{

    pcfg2 = 1; // configure A/D for AN0 (GP0) - Not used in this example
    pcfg1 = 1; // others as IO
    pcfg0 = 0;

    while(1)
    {
       ser_init();

       // set up TMR0
       t0cs = 0; // fosc / 4 is clock source
       psa = 1;  // prescale assigned to WDT
       ps2 = 0;  // 1:1 prescale
       ps1 = 0;
       ps0 = 0;

       // configure external int on GP2/INT
       intedg = 0; // 1 to 0 transition

       gp2 = 1;  // charge the capacitor
       tris2 = 0;

       delay_ms(10);

       count_hi = 0;
       count_lo = 0;

       tris2 = 1;  //  GP2 is a high impedance input
       TMR0 = 0x00;
       t0if = 0;  // kill any pening interrupt
       t0ie = 1;

       ext_int_occ = FALSE;
       intf = 0;
       inte = 1;

       gie = 1;

       while(1) // wait for cap to discharge
       {
          if(count_hi == 0xff) // no negative going transition
          {
             while(gie)
             {
                gie = 0;
             }
             count_lo = 0xff;
             break;
          }
          if (ext_int_occ)  // there was a negative going transition
          {
             while(gie)
             {
                gie = 0;
             }
             break;
          }
       }
       ser_init();
       ser_hex_byte(count_hi);
       ser_hex_byte(count_lo);
       delay_ms(1000);
    }
}

#int_rtcc tmr0_int_handler(void)
{
   ++count_hi;
}

#int_ext ext_int_handler(void)
{
   count_lo = TMR0;
   ext_int_occ = TRUE;
}

#int_default default_int_handler(void)
{
}

#include <delay.c>
#include <ser_672.c>

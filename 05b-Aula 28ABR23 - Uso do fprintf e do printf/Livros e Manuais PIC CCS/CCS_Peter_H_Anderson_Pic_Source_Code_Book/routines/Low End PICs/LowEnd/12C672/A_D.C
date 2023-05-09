// Program A_D.C, PIC12C672, CCS PCM
//
// Continually perform A/D conversions on AN0 on GP0.
// Displays results on Serial LCD or PC at 9600 baud using GP1.
//
//   +5                     PIC16C672
//    |
//    10K Pot <------------ GP0/AN0 (term 7)
//    |                     GP1 (term 6) --------------------> To Ser LCD or PC COM Port
//   GRD
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

byte ad_meas(void);
void calibrate(void);

main()
{
   byte ad_val;

   not_gppu = 0; // enable weak pullups
//    calibrate();  // do not use for emulation
   ser_init();
   while (1)  // continually
   {
      ad_val = ad_meas();
      ser_hex_byte(ad_val);
      ser_new_line();
      delay_ms(1000);
  }

} // end of main

byte ad_meas(void)
{
   pcfg2=1; // config for 1 analog channel on GP0
   pcfg1=1;
   pcfg1=0;

   adcs1=1;
   adcs0=1; // internal RC

   adon=1;  // turn on the A/D

   chs1 = 0; // channel 0
   chs0 = 0;

   delay_ms(1);

   adif=0; // reset the flag
   adie=1; // enable interrupts
   peie=1;
   gie=1;
   delay_10us(10);
   go_done = 1;
#asm
   CLRWDT
   SLEEP // turn of most of PIC to reduce noise during conversion
#endasm
   while(gie)  // turn off interrupts
   {
      gie=0;
   }

   adie=0; // not really necessary, but good practice.
   peie=0;
   return(ADRES);
}

void calibrate(void)
{
#asm
   CALL 0x03ff
   MOVWF OSCCAL
#endasm
}

#int_ad a_d_int_handler(void)
{

}

#int_default default_int_handler(void)
{
}

#include <delay.c>
#include <ser_672.c>

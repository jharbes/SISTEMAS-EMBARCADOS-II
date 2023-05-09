// Program AD_1.C
//
// Illustrates the use of the A/D using polling of the adgo bit.  Continually measures
// voltage on potentiometer on AN0 (term 2) and displays A/D value and angle.
//
// Uses macro to combine two bytes into a long.
//
// copyright, Peter H. Anderson, Baltimore, MD, Jan, '02

#case
#device PIC18C452

#include <defs_18c.h>
#include <delay.h>
#include <ser_18c.h>

#define FALSE 0
#define TRUE !0

#define MAKE_LONG(h, l)  (((long) h) << 8) | (l)

main()
{

   long ad_val;
   float angle;

   ser_init();

   pcfg3 = 0; pcfg2 = 1; pcfg1 = 0; pcfg0 = 0;
   // config A/D for 3/0

   adfm = 1;   // right justified
   adcs2 = 0; adcs1 = 1; adcs0 = 1; // internal RC

   adon=1;  // turn on the A/D
   chs2=0;  chs1=0;   chs0=0;
   delay_10us(10);      // a brief delay to allow channel to settle
   while(1)
   {
      adgo = 1;
      while(adgo)    ;  // poll adgo until zero

      ad_val = MAKE_LONG(ADRESH, ADRESL);
      angle = (float) ad_val * 270.0 / 1024.0;

      printf(ser_char, "ad_val = %ld, angle = %2.1f\n\r", ad_val, angle);

      delay_ms(3000);   // three second delay
   }
}

#include <delay.c>
#include <ser_18c.c>

// Program PWM_1.C
//
// Varies PWM duty using potentiometer on A/D Ch0 and outputs
// the value of "duty" to LCD.
//
//  PIC18C452
//
//  PORTC2 (term 17) -------- transistor ----------- motor -------- +V
//
// Uses 8-bit PWM.  The period is 1/256 us or about 4KHz.
//
// copyright, Peter H. Anderson, Baltimore, MD, Jan, '02

#case
#device PIC18C452

#include <defs_18c.h>
#include <delay.h>
#include <ser_18c.h>

main()
{
   byte duty;

   ser_init();

   // set up A/D converter
   pcfg3 = 0; pcfg2 = 1; pcfg1 = 0; pcfg0 = 0;
   // config A/D for 3/0
   adfm = 0;   // left justified - high 8 bits in ADRESH
   adcs2 = 0; adcs1 = 1; adcs0 = 1; // internal RC
   adon=1;  // turn on the A/D
   chs2=0;  chs1=0;   chs0=0;

   delay_10us(10);      // a brief delay

   // Configure CCP1
   PR2 = 0xff;    // period set to max of 256 usecs  - about 4 kHz
   duty = 0x00;
   CCPR1L = duty;  // duty initially set to zero

   // configure CCP1 for PWM operation
   ccp1m3 = 1;  ccp1m2 = 1;		// other bits are don't care

   // Timer 2 post scale set to 1:1 - not really necessary in this application
   toutps3 = 0;  toutps2 = 0; toutps1 = 0; toutps0 = 0;

   // Timer 2 prescale set to 1:1
   t2ckps1 = 0; t2ckps0 = 0;
   tmr2on = 1; // turn on timer #2

   latc2 = 0;
   trisc2 = 0; // make PORTC.2 an output 0

   while(1)
   {
      adgo = 1;
      while(adgo)    ;  // poll adgo until zero
      duty = ADRESH;
      CCPR1L = duty;
      printf(ser_char, "%x\r", duty);
      delay_ms(100);	// so the user can see the display
   }
}

#include <delay.c>
#include <ser_18c.c>

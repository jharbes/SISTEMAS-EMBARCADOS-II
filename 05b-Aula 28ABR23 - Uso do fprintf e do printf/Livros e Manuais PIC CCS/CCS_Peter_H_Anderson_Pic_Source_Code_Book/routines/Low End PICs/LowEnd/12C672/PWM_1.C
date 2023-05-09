// PWM_1.C (PIC12C672), CCS PCM
//
// Generates PWM at nominally 1 kHz on GP1 controlled by the setting of a
// potentiometer on GP0 / AN0.
//
// If input GP3 is at logic one (or open) output on GP1 is zero.
//
// For A/D results lower than 0x10, the output on GP1 is maintained at zero.
// Similarly, for A/D results above 0xf0, the output is maintained at a constant
// logic one.
//
//   +5                     PIC16C672
//    |
//    10K Pot <------------ GP0/AN0 (term 7)
//    |
//  GRD
//
//   GRD ---- \------------ GP3 (term 4) (if open, PWM output is zero)
//
//                          GP1 (term 6) (PWM output) --------- To FET or similar
//
// copyright, Peter H. Anderson, Elmore, VT, July, '01

#case

#device PIC12C672

#include <defs_672.h>

#include <delay.h>

#define TRUE !0
#define FALSE 0

void calibrate(void);
void setup_ad(void);
void setup_tmr0(void);

byte on_time, off_time;

void main(void)
{
   byte adval;
//   calibrate();  // do calibrate when using emulator
   not_gppu = 0;

   gp1 = 0;   // PWM off
   tris1 = 0;

   setup_ad();
   setup_tmr0();

   while(1)
   {
#asm
       CLRWDT
#endasm
    adgo = 1;
    while(adgo)
    {
    }

    adval = ADRES;
    off_time = adval;
    on_time = (~adval) + 1;

    if ((adval < 0x10) || (gp3)) // if low value or if gp3 is high
    {
       while(gie)
       {
          gie = 0;
       }
       gp1 = 0;  // off
       t0ie = 0;
       t0if = 0;
    }

    else if (adval > 0xf0)
    {
       while(gie)
       {
         gie = 0;
       }
       gp1 = 1;  // full on
       t0ie = 0;
       t0if = 0;
    }

    else
    {
       t0ie = 1;
       gie = 1;
    }
  }
}

void setup_ad(void)
{
   pcfg2=1; // config for 1 analog channel on GP0
   pcfg1=1;
   pcfg0=0;

   adcs1=1;
   adcs0=1; // internal RC

   adon=1;  // turn on the A/D

   chs1 = 0; // channel 0
   chs0 = 0;
   delay_ms(1);
}

void setup_tmr0(void)
{
   t0cs = 0; // f_osc / 4
   psa = 0; // prescaler assigned to TMR0
   ps2 = 0; // prescale is 1:4, 1.024 ms rollover
   ps1 = 0;
   ps0 = 1;
}

void calibrate(void)
{
#asm
   CALL 0x03ff
   MOVWF OSCCAL
#endasm
}

#int_rtcc tmr0_int_handler(void)
{
   gp1 = !gp1;
   if(gp1)
   {
     TMR0 = on_time;
   }
   else
   {
     TMR0 = off_time;
   }
}

#int_default default_int_handler(void)
{
}

#include <delay.c>

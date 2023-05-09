// 32KHZ.C 12C672, (CCS PCM)
//
// Pressure Alarm.  Intened to monitor that a venilation fan is continually on.
//
// Monitors pressure using an Motorola MPX5010 by measuring the A/D value on GP0.  If the
// pressure drops below ALARM_THRESH for TIME_OUT seconds, an alarm is sounded.  If at
// any time during the timing the pressure returns to normal, the program returns to the idle -
// no alarm state.
//
// The program remains in the alarm state if the pressure remains low.  If the pressure returns
// for 10 seconds, the program returns to the idle - no alarm state.
//
// If at any time, a pushbutton on input GP3 is depressed, the program returns to the HOME state and
// simply loops.
//
// A LED on GP2 is used for debugging to determine which state the program is in;
//
//       Idle (GP3 at ground)   1-blip
//       Idle - No Alarm        2
//       Timing to Alarm        3
//       Alarm                  4
//       Timing from Alarm      5
//
// Uses 32.768 kHz external clock.
//
//  MPX5010 ----------------- GP0/AN0 (term 7)
//
//  GRD --------------------- GP3 (term 4) (internal weak pullup)
//                            GP1 (term 6) ------------------------- ALARM --------- GRD
//
//                            GP2 (term 5) ---------- 330 ----->|----- GRD
//
// copyright, Peter H. Anderson, Baltimore, MD, Aug, '01

#case

#device PIC12C672

#include <defs_672.h>

#define TRUE !0
#define FALSE 0

#define TEST

#ifdef TEST
void test_blip(byte num_blips);  // used for debugging
#endif

byte ad_meas(void);
void alarm_burst(byte num_bursts);
void calibrate(void);
void delay_32kHz_ms(byte ms);

byte et_secs;

#define TIME_OUT 30 // number of secs for alarm
#define ALARM_THRESH 0x50

void main(void)
{
   byte ad_val;
   not_gppu = 0;

HOME:
   while(!gp3)  //  idle if gp3 is at ground
   {
#ifdef TEST
      test_blip(1);
#endif
#asm
       CLRWDT
#endasm
       t0ie = 0;
       adie = 0;
       while(gie)
       {
         gie = 0;
       }
       gp1 = 0;
   }

IDLE:
   while(1)   // operational state - no alarm
   {
#ifdef TEST
      test_blip(2);
#endif

#asm
       CLRWDT
#endasm


       if (!gp3)
       {
          goto HOME;
       }
       gp1 = 0; // be sure alarm is off
       tris1 = 0;
       ad_val = ad_meas();
       if (ad_val < ALARM_THRESH)
       {
          break;
       }
   }

TIMING_TO_ALARM:
   // set up TMR0
   t0cs = 0; // fosc / 4 is clock source
   psa = 0;  // prescale assigned to TMR0
   ps2 = 1;  // 1:32 prescale
   ps1 = 0;
   ps0 = 0;

   et_secs = 0;
   TMR0 = 0;
   t0if = 0;
   t0ie = 1;
   gie = 1;

   while(1)   // operational state - alarm, but not timed out
   {
#ifdef TEST
      test_blip(3);
#endif
#asm
       CLRWDT
#endasm
       if (!gp3)
       {
          goto HOME;
       }
       gp1 = 0; // be sure alarm is off
       tris1 = 0;
       ad_val = ad_meas();
       if (ad_val >= ALARM_THRESH)
       {
          goto HOME;
       }

       if (et_secs >= TIME_OUT)
       {
          while(gie)
          {
             gie = 0;
          }
          break;  // to ALARM
       }
   }

ALARM_STATE:  // alarm state
   while(1)
   {
#ifdef TEST
      test_blip(4);
#endif
#asm
      CLRWDT
#endasm
      if (!gp3)
      {
         goto HOME;
      }
      ad_val = ad_meas();
      if (ad_val >= ALARM_THRESH)  // alarm condition appears to be resolved
      {
          break; // to timing from alarm
      }
      alarm_burst(10);
   }

TIMING_FROM_ALARM:
   // set up TMR0
   t0cs = 0; // fosc / 4 is clock source
   psa = 0;  // prescale assigned to TMR0
   ps2 = 1;  // 1:32 prescale
   ps1 = 0;
   ps0 = 0;

   et_secs = 0;
   TMR0 = 0;
   t0if = 0;
   t0ie = 1;
   gie = 1;

   while(1)        // alarm cleared
   {
#ifdef TEST
      test_blip(5);
#endif

#asm
      CLRWDT
#endasm
      if (!gp3)
      {
        goto HOME;
      }
      ad_val = ad_meas();
      if (ad_val < ALARM_THRESH)  // alarm condition again present
      {
         goto ALARM_STATE;
      }
      if (et_secs >= 10)
      {
         goto HOME;     // alram is cleared
      }
   }
}

#ifdef TEST
void test_blip(void num_blips)
{
   byte n;

   gp2 = 0;
   tris2 = 0;  // make it an output

   for (n=0; n<num_blips; n++)
   {
      gp2 = 1;
      delay_32kHz_ms(100);
      gp2 = 0;
      delay_32kHz_ms(100);
   }

   delay_32kHz_ms(250);
   delay_32kHz_ms(250);
}
#endif

void alarm_burst(byte num_bursts)
{
   byte n;
   gp1 = 0;
   tris1 = 0;
   for(n = 0; n < num_bursts; n++)
   {
      gp1 = 1;
      delay_32kHz_ms(100);
      gp1 = 0;
      delay_32kHz_ms(100);
   }
}

void delay_32kHz_ms(byte ms)
{
   while(ms--)
   {
#asm
      CLRWDT
      NOP
      NOP
      NOP
      NOP
      NOP
#endasm
   }
}

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

   delay_32kHz_ms(1);

   go_done = 1;
   while(go_done)
   {
   }
   return(ADRES);
}

#int_rtcc tmr0_int_hander(void)
{
 ++et_secs;
}

#int_default default_int_handler(void)
{
}




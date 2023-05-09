// FRST_ALM.C  PIC12C672, CCS PCM
//
// Frost Alarm.
//
// When pushbutton on GP3 is at ground, a pot on GP0 is read.  This is mapped into a
// T_threshold in the range of 32 - 42 degrees or 81 degrees F and the value is displayed
// by flashing LED_ALM on GP5.
//
// Otherwise, the temperature is measured using a DS18S20 on GP2 and this is displayed
// by flashing LED_TEMP on GP4.
//
// If the measured temperature is less than the alarm temperature, a sonarlert is pulsed.
//
//
//    +5                     PIC16C672
//    |
//    10K Pot <------------ GP0/AN0 (Term 7)  (Alarm Threshold Adjust)
//    |
//    GRD
//		                 +5V
//		                  |
//		                  4.7K
//		                  |
//        GP2 (term 5) -------------- DS18S20
//
//                                     BiColor LED
//                          GP4 (term 3) ----->|----
//                                                  |_____ 330 ___ GRD
//                                                  |
//                          GP5 (term 2) ----->|---
//
//                          GP1 (term 6)  ------------> Sonalert
//
//          GRD ------ \--- GP3 (term 4)
//
//
// copyright, Peter H. Anderson, Baltimore, MD, Aug, '01

#case

#device PIC12C672

#include <defs_672.h>

#include <delay.h>
#include <_1_wire.h>

#define TRUE !0
#define FALSE 0

#define _1W_PIN 2
#define LED_ALM   5
#define LED_TEMP  4

void calibrate(void);

void alarm(void);
byte meas_threshold(void);
void meas_temperature(byte *p_T_C, byte *p_sign);

void flash_q(byte LED, byte q, byte minus_flag);

void main(void)
{
   byte T_threshold, T_C, T_F, sign;
   float T_C_float, T_F_float;

//   calibrate();  // not when using emulator

   pcfg2 = 1; // configure A/D for AN0 (GP0) - Not used in this example
   pcfg1 = 1; // others as IO
   pcfg0 = 0;

   while(1)
   {
      not_gppu = 0;
      gp1 = 0; // be sure sonalert is off

      T_threshold = meas_threshold();
      if (!gp3)		// set the alarm threshold
      {
         flash_q(LED_ALM, T_threshold, FALSE);
         delay_ms(1000);
      }

      else
      {
         meas_temperature(&T_C, &sign);

         if (sign) // its negative
         {
            T_C = (~T_C) + 1;
            T_C_float = (float) T_C * -0.5;
         }
         else
         {
            T_C_float = (float) T_C * 0.5;
         }
         T_F_float = 1.8 * T_C_float + 32.0;
         T_F = (byte) T_F_float;

         flash_q(LED_TEMP, T_F, FALSE);

         if (T_F < T_threshold) // pulse sonalert and LED
         {
            alarm();
         }

         else
         {
            delay_ms(2000);
         }
      } // end of else
   } // end of while 1
}

byte meas_threshold(void)
{
   byte T_thresh, adval;

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
   adgo = 1;
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
    adval = ADRES;

    if (adval > 0xf0) // this is for testing at room temperature
    {
       return(81); // degrees F
    }

    else
    {
       T_thresh = (0xd0 - adval) / 20 + 32; // 32 - 42 degrees
       return(T_thresh);
    }
}

void meas_temperature(byte *p_T_C, byte *p_sign)
{
   _1w_init(_1W_PIN);
   _1w_out_byte(_1W_PIN, 0xcc);  // skip ROM
   _1w_out_byte(_1W_PIN, 0x44);  // perform temperature conversion

   _1w_strong_pull_up(_1W_PIN);

   _1w_init(_1W_PIN);
   _1w_out_byte(_1W_PIN, 0xcc);  // skip ROM
   _1w_out_byte(_1W_PIN, 0xbe);

  *p_T_C = _1w_in_byte(_1W_PIN);
  *p_sign = _1w_in_byte(_1W_PIN);
}

void alarm(void)
{
    byte n;
    tris1 = 0;

    for (n = 0; n < 5; n++)
    {
       gp1 = 1;
       delay_ms(100);
       gp1 = 0;
       delay_ms(100);
    }
}

void flash_q(byte LED, byte q, byte minus_flag)
{
   byte n, digit;

#asm
   BCF GPIO, LED_ALM    // make LED pins output logic zeros
   BCF GPIO, LED_TEMP
   BCF TRISIO, LED_ALM
   BCF TRISIO, LED_TEMP
#endasm

   if (minus_flag)
   {
       if (LED == LED_ALM)
       {
#asm
          BSF GPIO, LED_ALM
#endasm
       }
       else
       {
#asm
          BSF GPIO, LED_TEMP
#endasm
       }

      delay_ms(500); // long delay to indicate minus
#asm
      BCF GPIO, LED_ALM
      BCF GPIO, LED_TEMP
#endasm
      delay_ms(1000);
   }

   digit = q/10; // number of tens
   if (digit)     // if non zero
   {
      for (n=0; n<digit; n++)
      {

         if (LED == LED_ALM)
         {
#asm
            BSF GPIO, LED_ALM
#endasm
         }
         else
         {
#asm
            BSF GPIO, LED_TEMP
#endasm
         }
         delay_ms(250);
#asm
         BCF GPIO, LED_ALM
         BCF GPIO, LED_TEMP
#endasm
      delay_ms(250);
      }

      delay_ms(1000); // separation between digits
   }

   digit = q%10;
   if (!digit)
   {
      digit = 10;
   }

   for (n=0; n<digit; n++)
   {
      if (LED == LED_ALM)
      {
#asm
         BSF GPIO, LED_ALM
#endasm
      }
      else
      {
#asm
          BSF GPIO, LED_TEMP
#endasm
      }
      delay_ms(250); // long delay to indicate minus
#asm
      BCF GPIO, LED_ALM
      BCF GPIO, LED_TEMP
#endasm
      delay_ms(250);
   }

   delay_ms(1000); // separation between digits
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
#include <_1_wire.c>


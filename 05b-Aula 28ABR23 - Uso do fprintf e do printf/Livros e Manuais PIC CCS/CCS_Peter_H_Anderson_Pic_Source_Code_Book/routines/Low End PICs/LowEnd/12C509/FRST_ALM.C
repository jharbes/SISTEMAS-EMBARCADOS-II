// FRST_ALM.C (PIC12C509), CCS-PCB
//
// Frost Alarm.
//
// Continually measures temperature using a Dallas DS18S20. If the temperature
// is negative (degrees C) or if T_C < T_threshold then a Sonalert on PORTC2
// is pulsed five times.
//
// The alarm threshold temperature T_thresh is measured using the RC configuration
// shown below.  If the time required for the capacitor to discharge to a logic zero
// exceeds nominally 65 ms as might be the case if the potentiometer is not present
// the T_thresold is 2 degrees C.  Otherwise, the RC time is mapped into T_threshold
// values in the range of 0 to 14 degrees C and 30 degrees C.  The 30 degrees C is
// provided for testing of the sonalert alarm.
//
// The program continually loops, displaying the values of T_threshold or T_C on Alarm
// LED or on Temperature LED.
//
//  PIC16C509
//								Bicolor LED
//   GP4 (term 3) ---------------->|---- |------ 330 ---- GRD
//   GP5 (term 2) ---------------->|---- |
//
//   GP2 (term 5) --- 330 ----- --------
//                               |        |
//                             1.0 uFd    10K Pot
//                               |        |
//                               |        10K Resistor
//                               |        |
//                              GRD       GRD
//                      +5VDC
//                       |
//                      4.7K
//                       |
//   GP1 (term 6) ---------------- DS18S20
//
//   GP0 (term 7) --------------- Sonalert ---- GRD
//
//   GRD ---- \---------- GP3 (term 4) (internal pullup resistor)
//
// copyright, Peter H. Anderson, Elmore, VT, July, '01

#case

#device PIC12C509

#include <defs_509.h>
#include <delay.h>

#define TRUE !0
#define FALSE 0

#define _1W_PIN 1
#define LED_ALM   5
#define LED_TEMP  4

void flash_q(byte LED, byte q, byte minus_flag);

byte meas_threshold(void);
byte meas_temperature(void);

void alarm(void);

// 1-wire prototypes
void w1_init(void);
int w1_in_byte(void);
void w1_out_byte(byte d);
void w1_strong_pull_up(void);

void main(void)
{
   byte T_C, T_threshold, minus_flag;

   DIRS = 0x3f;
#asm
   BCF GPIO, LED_ALM    // make LED pins output logic zeros
   BCF GPIO, LED_TEMP
   BCF DIRS, LED_ALM
   BCF DIRS, LED_TEMP
   MOVF DIRS, W
   TRIS GPIO
#endasm

   not_gppu = 0;  // enable internal weak pull-ups
#asm
   MOVF OPTIONS, W
   OPTION
#endasm

   while(1)
   {

      // measure the alarm threshold
      T_threshold = meas_threshold();
      if (!gp3)
      {
         flash_q(LED_ALM, T_threshold, FALSE);
         delay_ms(2000);
      }
      else
      {
          T_C = meas_temperature();

          if (T_C & 0x80)  // if negative
          {
             minus_flag = TRUE;
             T_C = (~T_C) + 1;
          }
          else
          {
              minus_flag = FALSE;
          }
          flash_q(LED_TEMP, T_C, minus_flag);

          if ((minus_flag) || (T_C < T_threshold))
          {
             alarm();
          }
          else
          {
             delay_ms(1000);
          }
      }
   }
}

void flash_q(byte LED, byte q, byte minus_flag)
{
   byte n, digit;

#asm
   BCF GPIO, LED_ALM    // make LED pins output logic zeros
   BCF GPIO, LED_TEMP
   BCF DIRS, LED_ALM
   BCF DIRS, LED_TEMP
   MOVF DIRS, W
   TRIS GPIO
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
         delay_ms(250); // long delay to indicate minus
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

void alarm(void)   // pulse sonalert five times
{
    byte n;
    gp0 = 0;
    dirs0 = 0;
#asm
    MOVF DIRS, W
    TRIS GPIO
#endasm

   for (n=0; n<5; n++)
   {
      gp0 = 1;
      delay_ms(100);
      gp0 = 0;
      delay_ms(100);
   }
}

byte meas_threshold(void)
{
    byte count_hi, count_lo, tmr0_old, tmr0_new, T_thresh;

    t0cs = 0; // fosc / 4 is clock source
    psa = 1;  // prescale assigned to WDT
    ps2 = 0;  // 1:1 prescale
    ps1 = 0;
    ps0 = 0;

#asm
    MOVF OPTIONS, W
    OPTION
#endasm

    dirs2 = 0;   // output
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
                return(2);
             }

        }

        if (!gp2) // if capacitor discharged below zero corssing
        {
            count_lo = tmr0_new;
            break;
        }
        tmr0_old = tmr0_new;
    }
    if (count_hi < 48)
    {
       count_hi = 48;
    }
    else if (count_hi > 96)
    {
       count_hi = 96;
    }

    if (count_hi > 90)
    {
       return(30);    // this is to test the alarm
    }

    else
    {
        return((count_hi - 48)/2);   // in the range of 0 to 21 degrees C
    }
}

byte meas_temperature(void)
{
    byte T_C, sign;

    w1_init();
    w1_out_byte(0xcc);  // skip ROM
    w1_out_byte(0x44);  // perform temperature conversion

    w1_strong_pull_up();

    w1_init();
    w1_out_byte(0xcc);  // skip ROM
    w1_out_byte(0xbe);  // read the result

    T_C = w1_in_byte();
    sign = w1_in_byte();

    if (sign)   // if negative, change to pos for divide by two
    {
       T_C = ~T_C + 1;
    }

    T_C = T_C / 2;

    if (sign)   // negative
    {
        T_C = ~T_C + 1;
    }
    return(T_C);
}

// The following are standard 1-Wire routines.
void w1_init(void)
{

#asm
      BSF DIRS, _1W_PIN  // high impedance
      MOVF DIRS, W
      TRIS GPIO

      BCF GPIO, _1W_PIN  // bring DQ low for 500 usecs
      BCF DIRS, _1W_PIN
      MOVF DIRS, W
      TRIS GPIO
#endasm
      delay_10us(50);
#asm
      BSF DIRS, _1W_PIN
      MOVF DIRS, W
      TRIS GPIO
#endasm
      delay_10us(50);
}

byte w1_in_byte(void)
{
   byte n, i_byte, temp;

   for (n=0; n<8; n++)
   {

#asm
       BCF GPIO, _1W_PIN // wink low and read
       BCF DIRS, _1W_PIN
       MOVF DIRS, W
       TRIS GPIO

       BSF DIRS, _1W_PIN
       MOVF DIRS, W
       TRIS GPIO

       CLRWDT
       NOP
       NOP
       NOP
       NOP
#endasm
       temp =  GPIO;  // now read
       if (temp & (0x01 << _1W_PIN))
       {
           i_byte=(i_byte>>1) | 0x80; // least sig bit first
       }
       else
       {
          i_byte=i_byte >> 1;
       }
       delay_10us(6);
   }

   return(i_byte);
}

void w1_out_byte(byte d)
{
   byte n;

   for(n=0; n<8; n++)
   {
      if (d & 0x01)
      {
#asm
          BCF GPIO, _1W_PIN  // wink low and high and wait 60 usecs
          BCF DIRS, _1W_PIN
          MOVF DIRS, W
          TRIS GPIO

          BSF DIRS, _1W_PIN
          MOVF DIRS, W
          TRIS GPIO
#endasm
          delay_10us(6);
      }

      else
      {
#asm
          BCF GPIO, _1W_PIN  // bring low, 60 usecs and bring high
          BCF DIRS, _1W_PIN
          MOVF DIRS, W
          TRIS GPIO
#endasm
          delay_10us(6);
#asm
          BSF DIRS, _1W_PIN
          MOVF DIRS, W
          TRIS GPIO
#endasm
       }
       d=d>>1;
    } // end of for
}


void w1_strong_pull_up(void) // bring DQ to strong +5VDC
{
#asm
    BSF GPIO, _1W_PIN // output a hard logic one
    BCF DIRS, _1W_PIN
    MOVF DIRS, W
    TRIS GPIO
#endasm

    delay_ms(750);
#asm
    BSF DIRS, _1W_PIN
    MOVF DIRS, W
    TRIS GPIO
#endasm
}


#include <delay.c>


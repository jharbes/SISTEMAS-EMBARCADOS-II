// FRST_ALM.C (PIC16C505), CCS PCB
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
// The program continually loops, displaying the values of T_threshold and T_C on a
// serial LCD or PC COM port at 9600, inverted.
//
//  PIC16C505
//   PORTC1 (term 9) --- 330 ----- --------
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
//   PORTB0 (term 13) ---------------- DS18S20
//   PORTC0 (term 10) ---------------- To Serial LCD or PC COM Port
//
//
//   PORTC2 (term 8) ----------------- Sonalert ---- GRD
//
//   GRD ---- \---------- PORTB3 (term 4)  See Text.
//
// copyright, Peter H. Anderson, Elmore, VT, July, '01

#case

#device PIC16C505 *=8

#include <defs_505.h>

#include <delay.h>
#include <ser_505.h>

#define TRUE !0
#define FALSE 0

#define TxData 0 // use PORTC0
#define INV // send inverted RS232

#define _1W_PIN 0

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

   DIRB = 0x3f;
   DIRC = 0x3f;

   while(1)
   {
      ser_init();
      // measure the alarm threshold
      T_threshold = meas_threshold();
      ser_dec_byte(T_threshold, 2);
      ser_new_line();

      T_C = meas_temperature();

      if (T_C & 0x80)  // if negative
      {
   minus_flag = TRUE;
         T_C = (~T_C) + 1;
         ser_char('-');
   }
   else
   {
   minus_flag = FALSE;
     }

      if (T_C > 9)
      {
         ser_dec_byte(T_C, 2);
      }
      else
      {
         ser_dec_byte(T_C, 1);
      }

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

void alarm(void)   // pulse sonalert five times
{
 byte n;
 portc2 = 0;
 dirc2 = 0;
#asm
    MOVF DIRC, W
    TRIS PORTC
#endasm

   for (n=0; n<5; n++)
   {
    portc2 = 1;
    delay_ms(100);
    portc2 = 0;
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

    dirc1 = 0;   // output
#asm
    MOVF DIRC, W
    TRIS PORTC
#endasm
    portc1 = 1; // charge capacitor
    delay_ms(10);

    count_hi = 0;
    count_lo = 0;

    tmr0_old = 0x00;
    TMR0 = 0x00;
#asm
    BSF DIRC, 1
    MOVF DIRC, W
    TRIS PORTC
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

        if (!portc1) // if capacitor discharged below zero corssing
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

   else if (count_hi > 90)
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
      BSF DIRB, _1W_PIN  // high impedance
      MOVF DIRB, W
      TRIS PORTB

      BCF PORTB, _1W_PIN  // bring DQ low for 500 usecs
      BCF DIRB, _1W_PIN
      MOVF DIRB, W
      TRIS PORTB
#endasm
      delay_10us(50);
#asm
      BSF DIRB, _1W_PIN
      MOVF DIRB, W
      TRIS PORTB
#endasm
      delay_10us(50);
}

byte w1_in_byte(void)
{
   byte n, i_byte, temp;

   for (n=0; n<8; n++)
   {

#asm
       BCF PORTB, _1W_PIN // wink low and read
       BCF DIRB, _1W_PIN
       MOVF DIRB, W
       TRIS PORTB

       BSF DIRB, _1W_PIN
       MOVF DIRB, W
       TRIS PORTB

       CLRWDT
       NOP
       NOP
       NOP
       NOP
#endasm
       temp =  PORTB;  // now read
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
      if (d&(0x01<< _1W_PIN))
      {
#asm
          BCF PORTB, _1W_PIN  // wink low and high and wait 60 usecs
          BCF DIRB, _1W_PIN
          MOVF DIRB, W
          TRIS PORTB

          BSF DIRB, _1W_PIN
          MOVF DIRB, W
          TRIS PORTB
#endasm
          delay_10us(6);
      }

      else
      {
#asm
          BCF PORTB, _1W_PIN  // bring low, 60 usecs and bring high
          BCF DIRB, _1W_PIN
          MOVF DIRB, W
          TRIS PORTB
#endasm
          delay_10us(6);
#asm
          BSF DIRB, _1W_PIN
          MOVF DIRB, W
          TRIS PORTB
#endasm
       }
       d=d>>1;
    } // end of for
}


void w1_strong_pull_up(void) // bring DQ to strong +5VDC
{
#asm
    BSF PORTB, _1W_PIN // output a hard logic one
    BCF DIRB, _1W_PIN
    MOVF DIRB, W
    TRIS PORTB
#endasm

    delay_ms(750);
#asm
    BSF DIRB, _1W_PIN
    MOVF DIRB, W
    TRIS PORTB
#endasm
}

#include <delay.c>
#include <ser_505.c>

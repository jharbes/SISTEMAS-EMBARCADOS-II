// FRST_ALM.C (PIC16HV540), CCS PCB
//
// Frost Alarm.
//
// Continually measures temperature using a Dallas DS18S20.  The temperature is displayed
// by sequentially displaying each digit on a common anode 7-segment LED.
//
// If the temperature is less than T_threshold, a Sonalert is pulsed.  T_threshold is set
// using a potentiometer in an RC network.
//
// When input PORTB7 is at ground, the T_threshold is displayed on the 7-seg LED.
//
//  PIC16HV540
//
// PORTA2 (term 1) --- 330 ----- --------
//                             |        |
//                           1.0 uFd    10K Pot
//                             |        |
//                             |        10K Resistor
//                             |        |
//                            GRD       GRD
//
// PORTA3 (term 2) ----    ( +5 VDC From PORTA3)
//                      |
//                     4.7K
//                      |
// PORTA1 (term 18) ---------------- DS18S20
// PORTA0 (term 17) ---------------- To Sonalert
//
//
// PORTB6 (term 12) ----- 1K ----------- a seg (term 1)   ----------- +12 VDC (term 3, 14)
// PORTB5 (term 11) ----- 1K ----------- b seg (term 13)
// PORTB4 (term 10) ----- 1K ----------- c seg (term 10)
// PORTB3 (term 9) ------ 1K ----------- d seg (term 8)
// PORTB2 (term 8) ------ 1K ----------- e seg (term 7)
// PORTB1 (term 7) ------ 1K ----------- f seg (term 2)
// PORTB0 (term 6) ------ 1K ----------- g seg (term 11)
//
//			     +12 VDC
//			         |
//			        10K
//			         |
//  GRD -------- \------------ PORTB7 (term 13), Ground to display T_thresh.
//
//          a   // Layout of 7-seg LED
//        f   b
//          g
//        e   c
//          d
//
// copyright, Peter H. Anderson, Elmore, VT, July, '01

#case

#device PIC16HV540

#include <defs_540.h>
#include <delay.h>

#define TRUE !0
#define FALSE 0

#define _1W_PIN 1

#define MINUS_SIGN 0x01 // segment g only
#define BLANK 0x00  // no segments

void config_processor(void);

void alarm(void);

void display_q(byte q, byte minus_flag);
void display_patt(byte patt);


// 1-wire prototypes
void _1w_init(void);
int _1w_in_byte(void);
void _1w_out_byte(byte d);
void _1w_strong_pull_up(void);

void main(void)
{
    byte T_C, T_F, sign, T_threshold, minus_flag, count_hi, count_lo, tmr0_new, tmr0_old;

    DIRA = 0x0f;
    DIRB = 0xff;

    config_processor();  // configure brownout, WDT, etc

    porta3 = 1;     // apply power to DS18S20 via RA3
    dira3 = 0;
#asm
    MOVF DIRA, W
    TRIS PORTA
#endasm
   while(1)
   {
      porta0 = 0;    // be sure sonalert is off

      // measure the alarm threshold
      t0cs = 0; // fosc / 4 is clock source
      psa = 1;  // prescale assigned to WDT
      ps2 = 0;  // 1:1 prescale
      ps1 = 0;
      ps0 = 0;

#asm
      MOVF OPTIONS1, W
      OPTION
#endasm

      dira2 = 0;   // output
#asm
      MOVF DIRA, W
      TRIS PORTA
#endasm
      porta2 = 1; // charge capacitor

      delay_ms(10);

      count_hi = 0;
      tmr0_old = 0x00;

      TMR0 = 0x00;
#asm
      BSF DIRA, 2
      MOVF DIRA, W
      TRIS PORTA
#endasm
      while(1) // wait for cap to discharge
      {
#asm
         CLRWDT
#endasm
         tmr0_new = TMR0;
         if ((tmr0_new < 0x80) && (tmr0_old >= 0x80))  // there was a roll over
         {
            ++count_hi;
            if (count_hi == 0) // no zero crossing with 65 ms
            {
               T_threshold = 2;
               goto MEAS_THRESH_DONE;
            }

         }
         if (!porta2) // if capacitor discharged below zero corssing
         {
             break;
         }
         // else
         tmr0_old = tmr0_new;
      }

      if (count_hi < 48)
      {
         count_hi = 48;
      }

      if (count_hi > 90)
      {
         T_threshold = 30;    // this is to test the alarm
         goto MEAS_THRESH_DONE;
      }

      T_threshold = ((count_hi - 48)/2);   // in the range of 0 to 21 degrees C

MEAS_THRESH_DONE:

      if (!portb7)      // simply display alarm threshold
      {
         display_patt(0x77);  // display an "A" as in alarm
         delay_ms(500);
         display_patt(BLANK);
         delay_ms(500);
         display_q(T_threshold, FALSE);
      }

      else              // otherwise, make a temperature meas and display
      {
         _1w_init();
         _1w_out_byte(0xcc);  // skip ROM

         _1w_out_byte(0x44);  // perform temperature conversion
         _1w_strong_pull_up();

         _1w_init();
         _1w_out_byte(0xcc);  // skip ROM
         _1w_out_byte(0xbe);  // read the result

         T_C = _1w_in_byte();
         sign = _1w_in_byte();

         if (sign)  // if negative
         {
             T_C = (~T_C) + 1;
         }

         T_C = T_C / 2;
         display_q(T_C, sign);

         if ((sign) || (T_C < T_threshold))
         {
            alarm();
         }

      }
      delay_ms(1000);
   }
}

// The following are standard 1-Wire routines.
void _1w_init(void)
{
#asm
      BSF DIRA, _1W_PIN  // high impedance
      MOVF DIRA, W
      TRIS PORTA

      BCF PORTA, _1W_PIN  // bring DQ low for 500 usecs
      BCF DIRA, _1W_PIN
      MOVF DIRA, W
      TRIS PORTA
#endasm
      delay_10us(50);
#asm
      BSF DIRA, _1W_PIN
      MOVF DIRA, W
      TRIS PORTA
#endasm
      delay_10us(50);
}

byte _1w_in_byte(void)
{
   byte n, i_byte, temp;

   for (n=0; n<8; n++)
   {

#asm
       BCF PORTA, _1W_PIN // wink low and read
       BCF DIRA, _1W_PIN
       MOVF DIRA, W
       TRIS PORTA

       BSF DIRA, _1W_PIN
       MOVF DIRA, W
       TRIS PORTA

       CLRWDT
       NOP
       NOP
       NOP
       NOP
#endasm
       temp =  PORTA;  // now read
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

void _1w_out_byte(byte d)
{
   byte n;

   for(n=0; n<8; n++)
   {
      if (d&0x01)
      {
#asm
          BCF PORTA, _1W_PIN  // wink low and high and wait 60 usecs
          BCF DIRA, _1W_PIN
          MOVF DIRA, W
          TRIS PORTA

          BSF DIRA, _1W_PIN
          MOVF DIRA, W
          TRIS PORTA
#endasm
          delay_10us(6);
      }

      else
      {
#asm
          BCF PORTA, _1W_PIN  // bring low, 60 usecs and bring high
          BCF DIRA, _1W_PIN
          MOVF DIRA, W
          TRIS PORTA
#endasm
          delay_10us(6);
#asm
          BSF DIRA, _1W_PIN
          MOVF DIRA, W
          TRIS PORTA
#endasm
       }
       d=d>>1;
    } // end of for
}

void _1w_strong_pull_up(void) // bring DQ to strong +5VDC
{
#asm
    BSF PORTA, _1W_PIN // output a hard logic one
    BCF DIRA, _1W_PIN
    MOVF DIRA, W
    TRIS PORTA
#endasm

    delay_ms(750);
#asm
    BSF DIRA, _1W_PIN
    MOVF DIRA, W
    TRIS PORTA
#endasm
}

void display_q(byte q, byte minus_flag)
{
   byte const hex_digit_patts[10] =  {0x7e, 0x30, 0x6d, 0x79, 0x33, 0x5b, 0x5f, 0x70, 0x7f, 0x7b};
   byte digit;

   if (minus_flag)
   {
      display_patt(MINUS_SIGN);
      delay_ms(500);
      display_patt(BLANK);
      delay_ms(500);
   }

   digit = q/10; // number of tens

   display_patt(hex_digit_patts[digit]);
   delay_ms(500);
   display_patt(BLANK);
   delay_ms(500);

   digit = q%10;

   display_patt(hex_digit_patts[digit]);
   delay_ms(500);
   display_patt(BLANK);
   delay_ms(500);
}

void display_patt(byte patt)
{

   patt = (~patt) & 0x7f; // convert to negative logic

   DIRB = DIRB & 0x80;  // make lowest seven bits outputs

#asm
    MOVF DIRB, W
    TRIS PORTB
#endasm

    PORTB = (PORTB & 0x80) | patt;
}

void alarm(void)   // pulse sonalert five times
{
   byte n;

#asm
   BCF PORTA, 0
   BCF DIRA, 0
   MOVF DIRA, W
   TRIS PORTA
#endasm

   for (n=0; n<10; n++)
   {
       porta0 = !porta0;
       delay_ms(100);
   }
}

void config_processor(void) // configure OPTION2 registers
{
    not_pcwu = 1; // wakeup disabled
    not_swdten = 1;
    rl = 1;   // regulated voltage is 5V
    sl = 1;   // sleep level same as RL
    not_boden = 1; // brownout disabled
#asm
    MOVF OPTIONS2, W
    TRIS 0x07
#endasm

}

#include <delay.c>

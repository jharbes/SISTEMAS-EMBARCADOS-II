// Program TST_LCD2.C (PIC16C505), CCS, PCB
//
// Interfaces with a DS18S20 on PORTB0.  Measures the temperature and displays on Hitachi
// text LCD on PORTC.
//
//   PIC16C505               			DMC20434
//
//    PORTC5 (term 5) -------------------- EN (CLK) (term 6)
//    PORTC4 (term 6) -------------------- RS (DAT/CMD) (term 4)
//                         GRD ----------- RW (term 5)
//
//    PORTC3 (term 7) -------------------- DB7 (term 14)
//    PORTC2 (term 8) -------------------- DB6 (term 13)
//    PORTC1 (term 9) -------------------- DB5 (term 12)
//    PORTC0 (term 10) ------------------- DB4 (term 11)
//
//                          +5 VDC ------- VCC (term 2)
//                                   |
//                                  4.7K
//                                   |____ VEE (term 3)
//          |
//                                  330
//                                   |
//                           GRD ---------- GRD (term 1)
//
//
//     					 +5VDC
//    					  |
//    					 4.7K
//    					  |
//  PORTB0 (term 13) --------------------------- DQ of DS18S20
//
// copyright, Peter H. Anderson, Brattleboro, VT, July, '01

#case

#device PIC16C505 *=8

#include <defs_505.h>
#include <lcd_out.h>
#include <delay.h>

#define _1W_PIN 0 // PORTB0

#separate byte meas_temperature(void);

// 1-wire prototypes
void w1_init(void);
int w1_in_byte(void);
void w1_out_byte(byte d);
void w1_strong_pull_up(void);

void main(void)
{
    byte T_C;

    DIRC = 0x3f;
    DIRB = 0x3f;

    lcd_init();

    while(1)
    {
       T_C = meas_temperature();

       lcd_clr_line(0);       // beginning of line 0
       if (T_C & 0x80)		  // if its negative
       {
     	  lcd_char('-');
          T_C = (~T_C) + 1;
       }

       lcd_dec_byte(T_C, 2);

       delay_ms(1000);
   }
}

#separate byte meas_temperature(void)
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

#include <lcd_out.c>
#include <delay.c>

// 1820_1.C (PIC16C505), CCS PCB
//
// Illustrates an implementation of Dallas 1-wire interface.
//
// Configuration.  DS18S20 on PORTB.0.  Note a 4.7K pullup to +5V is required.
// DS18S20s configured in parasite power mode. That is, VCC connected to ground.
//
// Reads and displays temperature and diplays the result on serial LCD
// (or PC COM Port) connected to PORTC.0.
//
// PIC16C505
//    					 +5VDC
//    					  |
//    					 4.7K
//    					  |
//  PORTB0 (term 13) --------------------------- DQ of DS18S20
//  PORTC0 (term 10) --------------------------- To Ser LCD or PC Com Port
//
// Debugged using RICE-17A Emulator, July 19, '01.
//
// Note that the one routines were previously named beginning with "_1w".  It
// appears that the use of a leading underscore suppresses the variables when using
// the RICE-17A emulator.  Thus, they were renamed beginning with "w1".
//
// copyright, Peter H. Anderson, Elmore, VT, July, '01

#case

#device PIC16C505 *=8

#include <defs_505.h>
#include <delay.h>
#include <ser_505.h>

#define TxData 0 // use PORTC0
#define INV // send inverted RS232

#define _1W_PIN 0 // PORTB0

// 1-wire prototypes
void w1_init(void);
int w1_in_byte(void);
void w1_out_byte(byte d);
void w1_strong_pull_up(void);

void main(void)
{
   byte T_C, sign;
   DIRB = 0x3f;
   DIRC = 0x3f;

   while(1)
   {
       ser_init();

       w1_init();
       w1_out_byte(0xcc);  // skip ROM
       w1_out_byte(0x44);  // perform temperature conversion

       w1_strong_pull_up();

       w1_init();
       w1_out_byte(0xcc);  // skip ROM
       w1_out_byte(0xbe);  // read the result

       T_C = w1_in_byte();
       sign = w1_in_byte();

       if (sign)   // negative
       {
            T_C = ~T_C + 1;
            ser_char('-');
       }

       T_C = T_C / 2;

      if (T_C > 99)
      {
         ser_dec_byte(T_C, 3);
      }
      else if (T_C > 9)
      {
         ser_dec_byte(T_C, 2);
      }
      else
      {
         ser_dec_byte(T_C, 1);
      }
      delay_ms(1000);
   }
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
      if (d&0x01)
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

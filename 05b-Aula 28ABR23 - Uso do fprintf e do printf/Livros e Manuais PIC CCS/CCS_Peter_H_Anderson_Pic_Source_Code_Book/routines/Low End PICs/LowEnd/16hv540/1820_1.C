// 1820_1.C (PIC16HV540), CCS PCB
//
// Illustrates an implementation of Dallas 1-wire interface.
//
// Configuration.  DS18S20 on PORTA.1.  Note a 4.7K pullup to +5V is required.
// DS18S20s configured in parasite power mode. That is, VCC connected to ground.
//
// Reads and displays temperature and diplays the result on serial LCD
// (or PC COM Port) connected to PORTA.0.
//
// PIC16HV540
//          	    +5VDC (From PORTA3 - Term 2))
//          			 |
//          			4.7K
//          			 |
//  PORTA1 (term 16) --------------------------- DQ of DS18S20
//  PORTA0 (term 17) --------------------------- To Ser LCD or PC Com Port
//
// copyright, Peter H. Anderson, Elmore, VT, July, '01

#case

#device PIC16HV540

#include <defs_540.h>
#include <delay.h>
#include <ser_540.h>

#define TxData 0 // use PORTC0
#define INV // send inverted RS232

#define _1W_PIN 1 // PORTA1

void config_processor(void);

// 1-wire prototypes
void _1w_init(void);
int _1w_in_byte(void);
void _1w_out_byte(byte d);
void _1w_strong_pull_up(void);

void main(void)
{
   byte T_C, sign;

   DIRA = 0x0f;
   DIRB = 0x3f;

   config_processor();

#asm
    BSF PORTA, 3   // apply power to DS18S20
    BCF DIRA, 3
    MOVF DIRA, W
    TRIS PORTA
#endasm
    delay_ms(1000);

    while(1)
    {
       ser_init();

       _1w_init();
       _1w_out_byte(0xcc);  // skip ROM
       _1w_out_byte(0x44);  // perform temperature conversion

       _1w_strong_pull_up();

       _1w_init();
       _1w_out_byte(0xcc);  // skip ROM
       _1w_out_byte(0xbe);  // read the result

       T_C = _1w_in_byte();
       sign = _1w_in_byte();

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
#include <ser_540.c>

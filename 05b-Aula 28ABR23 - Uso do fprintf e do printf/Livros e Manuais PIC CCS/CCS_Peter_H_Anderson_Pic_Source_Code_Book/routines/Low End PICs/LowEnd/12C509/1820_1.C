// 1820_1.C, CCS - PCB  (PIC12C509)
//
// Illustrates an implementation of Dallas 1-wire interface.
//
// Configuration.  DS18S20 on GP.1 and GP.2.  Note a 4.7K pullup to +5V
// is required.  DS18S20s configured in parasite power mode. That is, VCC
// connected to ground.
//
// Reads and displays temperature and diplays the result on serial LCD
// (or PC COM Port) connected to PORTC.0.
//
// PIC12C509
//
//  GP2 (term 5) --------------------------- DQ of DS18S20
//  GP1 (term 6) --------------------------- DQ of DS18S20
//  GP0 (term 7) --------------------------- To Ser LCD or PC Com Port
//
// 4.7K Pullup Resistors to +5 VDC on DQ leads of each DS18S20.
//
// Debugged using RF Solutions ICEPIC Emulator, July 27, '01.
//
// copyright, Peter H. Anderson, Elmore, VT, July, '01

#case

#device PIC12C509 	// Problem with bank using ICEPIC emulator
					// Unsure if compiler or emulator

#include <defs_509.h>

#include <delay.h>
#include <ser_509.h>

#define TxData 0 // use GP0
#define INV // send inverted RS232

// 1-wire prototypes
void _1w_init(byte sensor);
int _1w_in_byte(byte sensor);
void _1w_out_byte(byte sensor, byte d);
void _1w_strong_pull_up(byte sensor);

void main(void)
{
   byte sensor, T_C, sign;
   DIRS=0x3f;

   while(1)
   {
      for (sensor=1; sensor<3; sensor++) // sensors 1 and 2 only
      {

         _1w_init(sensor);
         _1w_out_byte(sensor, 0xcc);  // skip ROM

        _1w_out_byte(sensor, 0x44);  // perform temperature conversion
        _1w_strong_pull_up(sensor);

        _1w_init(sensor);
        _1w_out_byte(sensor, 0xcc);  // skip ROM
        _1w_out_byte(sensor, 0xbe);  // read the result

        T_C = _1w_in_byte(sensor);
        sign = _1w_in_byte(sensor);

        ser_init();
        ser_dec_byte(sensor, 1); // display the sensor number
        ser_new_line();

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
        delay_ms(2000);
     }
   }
}

// The following are standard 1-Wire routines.
void _1w_init(byte sensor)
{
   if (sensor==1) // there probably is a more efficient technique
                  // my goal here was clarity
   {
#asm
      BSF DIRS, 1 // high impedance
      MOVF DIRS, W
      TRIS GPIO

      BCF GPIO, 1 // bring DQ low for 500 usecs
      BCF DIRS, 1
      MOVF DIRS, W
      TRIS GPIO
#endasm
      delay_10us(50);
#asm
      BSF DIRS, 1
      MOVF DIRS, W
      TRIS GPIO
#endasm
      delay_10us(50);
   }

   else  // its channel 2
   {
#asm
      BSF DIRS, 2
      MOVF DIRS, W
      TRIS GPIO

      BCF GPIO, 2 // bring DQ low for 500 usecs
      BCF DIRS, 2
      MOVF DIRS, W
      TRIS GPIO
#endasm
      delay_10us(50);
#asm
      BSF DIRS, 2
      MOVF DIRS, W
      TRIS GPIO
#endasm
      delay_10us(50);
   }
}

byte _1w_in_byte(byte sensor)
{
   byte n, i_byte, temp;

   for (n=0; n<8; n++)
   {
      if (sensor==1)
      {
#asm
         BCF GPIO, 1 // wink low and read
         BCF DIRS, 1
         MOVF DIRS, W
         TRIS GPIO

         BSF DIRS, 1
         MOVF DIRS, W
         TRIS GPIO

         CLRWDT
         NOP
         NOP
         NOP
         NOP
#endasm
         temp=GPIO;  // now read
         if (temp & 0x02)  // if GP1
         {
             i_byte=(i_byte>>1) | 0x80; // least sig bit first
         }
         else
         {
           i_byte=i_byte >> 1;
         }
         delay_10us(6);
      }
      else
      {
#asm
         BCF GPIO, 2
         BCF DIRS, 2
         MOVF DIRS, W
         TRIS GPIO

         BSF DIRS, 2
         MOVF DIRS, W
         TRIS GPIO

         CLRWDT
         NOP
         NOP
#endasm
         temp=GPIO;
         if (temp & 0x04)  // GP.2
         {
             i_byte=(i_byte>>1) | 0x80; // least sig bit first
         }
         else
         {
           i_byte=i_byte >> 1;
         }
         delay_10us(6);
      }
   }
   return(i_byte);
}

void _1w_out_byte(byte sensor, byte d)
{
   byte n;
   if (sensor==1)
   {
      for(n=0; n<8; n++)
      {
         if (d&0x01)
         {
#asm
            BCF GPIO, 1  // wink low and high and wait 60 usecs
            BCF DIRS, 1
            MOVF DIRS, W
            TRIS GPIO

            BSF DIRS, 1
            MOVF DIRS, W
            TRIS GPIO
#endasm
            delay_10us(6);
         }

         else
         {
#asm
            BCF GPIO, 1  // bring low, 60 usecs and bring high
            BCF DIRS, 1
            MOVF DIRS, W
            TRIS GPIO
#endasm
            delay_10us(6);
#asm
            BSF DIRS, 1
            MOVF DIRS, W
            TRIS GPIO
#endasm
         }
         d=d>>1;
      } // end of for
   }
   else  // sensor 2
   {
      for(n=0; n<8; n++)
      {
         if (d&0x01)
         {
#asm
            BCF GPIO, 2
            BCF DIRS, 2
            MOVF DIRS, W
            TRIS GPIO

            BSF DIRS, 2
            MOVF DIRS, W
            TRIS GPIO
#endasm
            delay_10us(6);
         }

         else
         {
#asm
            BCF GPIO, 2
            BCF DIRS, 2
            MOVF DIRS, W
            TRIS GPIO
#endasm
            delay_10us(6);
#asm
            BSF DIRS, 2
            MOVF DIRS, W
            TRIS GPIO
#endasm
         }
         d=d>>1;
      } // end of for
   }
}


void _1w_strong_pull_up(byte sensor) // bring DQ to strong +5VDC
{
   if (sensor ==1)
   {
#asm
      BSF GPIO, 1 // output a hard logic one
      BCF DIRS, 1
      MOVF DIRS, W
      TRIS GPIO
#endasm

      delay_ms(750);
#asm
      BSF DIRS, 1
      MOVF DIRS, W
      TRIS GPIO
#endasm
   }
   else
   {
#asm
      BSF GPIO, 2 // output a hard logic one
      BCF DIRS, 2
      MOVF DIRS, W
      TRIS GPIO
#endasm

      delay_ms(750);
#asm
      BSF DIRS, 2
      MOVF DIRS, W
      TRIS GPIO
#endasm
   }
}


#include <delay.c>
#include <ser_509.c>

// Program FIRST_TM.C, (12CE674)
//
// Illustrates the use of internal EEPROM.
//
// Four locations 0x0c - 0x0f in the internal EEPROM are used to determine if this
// is the first time the processor has been booted by checking for the pattern
// 0x78, 0x87, 0xa5, 0x5a.  If the pattern is not present, the routine writes this
// pattern to locations 0x0c - 0x0f.
//
// Location 0x00 in EEPROM is used to store the number of times the processor has been booted.
//
// Serial LCD is connected to GP0.  Serial data is 9600 baud, inverted.
//
// copyright, Peter H. Anderson, Elmore, VT, July, '01

#case

#device PIC12CE674

#include <defs_672.h>
#include <string.h> // for strcpy

#include <delay.h>
#include <ser_672.h>

#define TxData 0 // use GP0
#define INV // send inverted RS232

#define TRUE !0
#define FALSE 0

byte i2c_internal_eeprom_random_read(byte adr);
void i2c_internal_eeprom_random_write(byte adr, byte dat);

// standard I2C routines for internal EEPROM
byte i2c_internal_in_byte(byte ack);
void i2c_internal_out_byte(byte o_byte);

void i2c_internal_start(void);
void i2c_internal_stop(void);
void i2c_internal_high_sda(void);
void i2c_internal_low_sda(void);
void i2c_internal_high_scl(void);
void i2c_internal_low_scl(void);

byte first_time(void);
void calibrate(void);

byte high_two_bits; // bits 7 and 6 of GPIO

void main(void)
{

   byte count;

   pcfg2 = 1; // configure A/D for AN0 (GP0) - Not used in this example
   pcfg1 = 1; // others as IO
   pcfg0 = 0;
//   calibrate();		// do not use this function during emulation


   high_two_bits = 0xc0; // bits 7 and 6 at one
   GPIO = GPIO & 0x3f | high_two_bits;

   ser_init();

   if (first_time())
   {
    count = 1;
   }
   else
   {
    count = i2c_internal_eeprom_random_read(0);
    ++count;
   }

 i2c_internal_eeprom_random_write(0, count);
   ser_dec_byte(count, 3);

   while(1)
   {
   }
}

byte first_time(void)
{
    byte i, j;
    byte const patts[4] = {0x78, 0x87, 0xa5, 0x5a};

    for (i = 0; i < 4; i++)
    {
       if (i2c_internal_eeprom_random_read(i+0x0c) != patts[i])  // if locations 0x0c - 0x0f different from patts
       {
          for (j = 0; j < 4; j++)
          {
             i2c_internal_eeprom_random_write(j+0x0c, patts[j]); // program the patts at 0x0c - 0x0f
          }
          return(TRUE);
       }
    }
    return(FALSE);
}

byte i2c_internal_eeprom_random_read(byte adr)
{
   byte d;
   i2c_internal_start();
   i2c_internal_out_byte(0xa0);
   i2c_internal_out_byte(adr);

   i2c_internal_start();
   i2c_internal_out_byte(0xa1);
   d = i2c_internal_in_byte(0);  // no ack prior to stop
   i2c_internal_stop();
   return(d);
}

void i2c_internal_eeprom_random_write(byte adr, byte dat)
{
   i2c_internal_start();
   i2c_internal_out_byte(0xa0);
   i2c_internal_out_byte(adr);
   i2c_internal_out_byte(dat);
   i2c_internal_stop();
   delay_ms(25); // wait for byte to burn
}

byte i2c_internal_in_byte(byte ack)
{
   byte i_byte, n;
   i2c_internal_high_sda();
   for (n=0; n<8; n++)
   {
      i2c_internal_high_scl();
      if (sda_in)
      {
         i_byte = (i_byte << 1) | 0x01; // msbit first
      }
      else
      {
         i_byte = i_byte << 1;
      }
      i2c_internal_low_scl();
   }
   if (ack)
   {
    i2c_internal_low_sda();
   }
   else
   {
    i2c_internal_high_sda();
   }
   i2c_internal_high_scl();
   i2c_internal_low_scl();

   i2c_internal_high_sda();
   return(i_byte);
}

void i2c_internal_out_byte(byte o_byte)
{
   byte n;
   for(n=0; n<8; n++)
   {
      if(o_byte&0x80)
      {
         i2c_internal_high_sda();
         //ser_char('1'); // used for debugging
      }
      else
      {
         i2c_internal_low_sda();
         //ser_char('0'); // used for debugging
      }
      i2c_internal_high_scl();
      i2c_internal_low_scl();
      o_byte = o_byte << 1;
   }
   i2c_internal_high_sda();

   i2c_internal_high_scl(); // provide opportunity for slave to ack
   i2c_internal_low_scl();
   //ser_new_line();   // for debugging
}

void i2c_internal_start(void)
{
   i2c_internal_low_scl();
   i2c_internal_high_sda();
   i2c_internal_high_scl(); // bring SDA low while SCL is high
   i2c_internal_low_sda();
   i2c_internal_low_scl();
}

void i2c_internal_stop(void)
{
   i2c_internal_low_scl();
   i2c_internal_low_sda();
   i2c_internal_high_scl();
   i2c_internal_high_sda();  // bring SDA high while SCL is high
   // idle is SDA high and SCL high
}

void i2c_internal_high_sda(void)
{
   high_two_bits = high_two_bits | 0x40; // X1
   GPIO = (GPIO & 0x3f) | high_two_bits;
   delay_10us(5);
}

void i2c_internal_low_sda(void)
{
   high_two_bits = high_two_bits & 0x80; // X0
   GPIO = (GPIO & 0x3f) | high_two_bits;
   delay_10us(5);
}

void i2c_internal_high_scl(void)
{
   high_two_bits = high_two_bits | 0x80; // 1X
   GPIO = (GPIO & 0x3f) | high_two_bits;
   delay_10us(5);
}

void i2c_internal_low_scl(void)
{
   high_two_bits = high_two_bits & 0x40; // 0X
   GPIO = (GPIO & 0x3f) | high_two_bits;
   delay_10us(5);
}

void calibrate(void)
{
#asm
   CALL 0x03ff
   MOVWF OSCCAL
#endasm
}

#include <delay.c>
#include <ser_672.c>




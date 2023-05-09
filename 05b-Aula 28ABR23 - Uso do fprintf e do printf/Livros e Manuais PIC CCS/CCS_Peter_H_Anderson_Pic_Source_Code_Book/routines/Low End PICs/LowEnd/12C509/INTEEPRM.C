// Program INTEEPRM.C, (PIC12CE519) CCS PCB
//
// Illustrates how to write to and read from internal EEPROM on the PIC12CE519.
//
// Note that I was unable successfully implement the MicroChip
// routine FL51XINC.ASM and as I don't have an emulator for the
// 12CE519, I managed to burn up some 30 12CE519's before abandoning
// that approach.
//
// This routine does proably use a bit more program memory but does
// work.
//
// Serial LCD or PC Com Port is connected to GP.0.  Serial data is 9600 baud, inverted.
//
// copyright, Peter H. Anderson, Elmore, VT, July, '01

#case

#device PIC12CE519

#include <defs_509.h>

#include <delay.h>
#include <ser_509.h>

#define TxData 0 // use GP0
#define INV // send inverted RS232

#separate byte i2c_internal_eeprom_random_read(byte adr);
#separate void i2c_internal_eeprom_random_write(byte adr, byte dat);

// standard I2C routines for internal EEPROM
byte i2c_internal_in_byte(byte ack);
void i2c_internal_out_byte(byte o_byte);
void i2c_internal_start(void);
void i2c_internal_stop(void);
void i2c_internal_high_sda(void);
void i2c_internal_low_sda(void);
void i2c_internal_high_scl(void);
void i2c_internal_low_scl(void);

byte high_two_bits; // bits 7 and 6 of GPIO

main(void)
{
   byte mem_adr, dat, m, n;

   high_two_bits = 0xc0; // bits 7 and 6 at one
   GPIO = GPIO & 0x3f | high_two_bits;

   DIRS = 0x3f;

   while(1)
   {
       ser_init();
       mem_adr=0x00;
       for(n=0; n<4; n++)
       {
         dat = 0x10 + n;
         i2c_internal_eeprom_random_write(mem_adr, dat);
         ++mem_adr;
       }

     // now, read the data back and display
       mem_adr=0x00;
       for(n=0; n<4; n++)
       {
         dat = i2c_internal_eeprom_random_read(mem_adr);
         ser_hex_byte(dat);
         ser_char(' ');
         ++mem_adr;
       }
       delay_ms(500);
    }
}

#separate byte i2c_internal_eeprom_random_read(byte adr)
{
   byte dat;
   i2c_internal_start();
   i2c_internal_out_byte(0xa0);
   i2c_internal_out_byte(adr);

   i2c_internal_start();
   i2c_internal_out_byte(0xa1);

   dat = i2c_internal_in_byte(0); // no ack prior to stop
   i2c_internal_stop();
   return(dat);
}

#separate void i2c_internal_eeprom_random_write(byte adr, byte dat)
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
      i2c_internal_low_sda(); // ack slave with zero
   }
   i2c_internal_high_scl();
   i2c_internal_low_scl();
   i2c_internal_high_sda(); // be sure to exit with SDA high
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
   //ser_new_line();   // for debugging
   i2c_internal_high_scl();    // allow for slave to ack
   i2c_internal_low_scl();
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

#include <delay.c>
#include <ser_509.c>

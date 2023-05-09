// Program INT_EE_1.C, (12CE674)
//
// Illustrates how to write to and read from internal EEPROM.
//
// Note that I was unable to successfully implement this using;
// scl = 0; sda = 1; etc.  Rather, a global variable "high_two_bits" was
// defined and bits 7 and 6 were set and cleared as appropriate and then
// output to GPIO as GPIO = GPIO & 0x3f | high_two_bits.
//
// My guess as to why the scl=0; sda=1; approach does not work is that
// I assume there are no TRIS bits associated with bits 7 and 6 of GPIO.
// Thus, in implementing scl=0; the PIC is actually reading SCL and SDA
// from the internal EEPROM and making SCL a zero.  Unfortunately, this
// may affect the state of SDA.  For example, assume SDA was a zero and
// the statement SCL=1 is implemented.  But, in doing so, the PIC may
// read a one from the internal EEPROM on SDA and the result is that not
// only is SCL a one, but SDA is also a one.
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

byte high_two_bits; // bits 7 and 6 of GPIO

void main(void)
{
   byte mem_adr, dat, n;

   //OSCCAL=_READ_OSCCAL_DATA();

   pcfg2 = 1;; // GP0, GP1, GP2, GP4 configured as general purpose IOs
   pcfg1 = 1;
   pcfg0 = 1;

   high_two_bits = 0xc0; // bits 7 and 6 at one
   GPIO = GPIO & 0x3f | high_two_bits;

   ser_init();

   while(1)
   {
      mem_adr=0x00;
      for(n=0; n<4; n++) // write four bytes to EEPROM
      {
         dat = n+10;
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
      ser_new_line();
      delay_ms(1000);
   } // end of while
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

#include <delay.c>
#include <ser_672.c>




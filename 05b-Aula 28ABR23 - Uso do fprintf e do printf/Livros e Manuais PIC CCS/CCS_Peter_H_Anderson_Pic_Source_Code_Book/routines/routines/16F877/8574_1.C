// 8574_1.C
//
// Illustrates control of 8574.  Flashes LED on P0 of 8574 if switch at
// P7 of 8574 is at zero.  Uses SSP Module in the I2C Master Mode.
//
//    PIC16F877				PCF8574
//
//  SCL/RC3 (term 18)---- SCL (term 14) ----- To Other
//  SDA/RC4 (term 23)---- SDA (term 15) ----- I2C Devices
//
// Note that the slave address is determined by A2 (term 3), A1
// (term 2) and A0 (term 1) on the 8574.  The above SCL and SDA leads
// may be multipled to eight devices, each strapped for a unique A2
// A1 A0 setting.  In this example, A2, A1 and A0 are strapped to ground.
//
// Pullup resistors to +5VDC are required on both SDA and SCL
// signal leads.
//
// copyright, Peter H. Anderson, Baltimore, MD, Mar, '01

#case

#device PIC16F877 *=16 ICD=TRUE

#include <defs_877.h>
#include <lcd_out.h>
#include <i2c_mstr.h>

#define TRUE !0
#define FALSE 0

// routines used for 8574
byte in_patt(byte dev_adr);
void out_patt(byte dev_adr, byte dirs, byte patt);

void main(void)
{
   byte inputs;

   i2c_master_setup();

   out_patt(0x00, 0x80, 0x7f);
   // address = 0, dirs = 0x80, patt = 0x7f

   while(1)
   {
      inputs = in_patt(0x00); // read inputs

      if (inputs&0x80)  // if switch at in7==1 then turn off LED
      {
         out_patt(0x00, 0x80, 0x7f);
      }
      else // flash the LED one time
      {
         out_patt(0x00, 0x80, 0x7e);  // turn the LED on
         delay_ms(500);
         out_patt(0x00, 0x80, 0x7f);  // turn it on
         delay_ms(500);
      }
   }
}

byte in_patt(byte dev_adr)
{
   byte y;
   i2c_master_start();
   i2c_master_out_byte(0x40 | (dev_adr<<1) | 0x01);
   y=i2c_master_in_byte(FALSE);
   i2c_master_stop();
   return(y);
}

void out_patt(byte dev_adr, byte dirs, byte patt)
{
   i2c_master_start();
   i2c_master_out_byte(0x40 | (dev_adr << 1));
   i2c_master_out_byte(dirs | patt);
   i2c_master_stop();
}

#include <lcd_out.c>
#include <i2c_mstr.c>

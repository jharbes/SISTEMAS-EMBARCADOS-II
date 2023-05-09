// Program MAX518_1.C,
//
// Illustrates an interface with a MAX518 Dual D/A.  This routine
// uses the MSSP Module as an I2C Master.
//
// Program outputs a constant 0x80 on D/A 0. The result is nominally
// 2.5V on OUT0.  In addition, outputs a triangular wave on D/A1.
//
//    PIC16F877				MAX518
//
// SCL/RC3 (term 18)----- SCL (term 3) ----- To Other
// SDA/RC4 (term 23)----- SDA (term 4) ----- I2C Devices
//
// Note that the slave address is 0101 1 AD1 AD0 where AD1 and AD0
// correspond to terminals 5 and 6, respectively.  In this program
// they are strapped to ground and thus the slave address is 0x58.
//
// External pullup resistors to +5VDC are not required on the SDA
// and SCL signal leads.  However, they may be present to maintain
// compatibility with other I2C devices.
//
// In command byte;
//          RST (bit 4) set to 1 resets both D/As
//	        PD (bit 3) set to one for power down
//          A0 (bit 0) identifies whether data is for D/A0 or D/A1
//
// copyright, Peter H. Anderson, Baltimore, MD, Mar, '01

#case

#device PIC16F877 *=16 ICD=TRUE

#include <defs_877.h>
#include <lcd_out.h>
#include <i2c_mstr.h>

#define TRUE !0
#define FALSE 0


void max518_d_a(byte dev_adr, byte d_a, byte dat);
// dev_adr is  AD1, AD0 strapping, d_a is either 0 or 1, dat is data
// to be output
void max518_power_down(byte dev_adr);


void main(void)
{
   byte i=0;
   byte const triangular[16]=
          {0x00, 0x20, 0x40, 0x60, 0x80, 0xa0, 0xc0, 0xe0,
           0xe0, 0xc0, 0xa0, 0x80, 0x60, 0x40, 0x20, 0x00};

   unsigned long j=5000;

   lcd_init();		// used for possible debugging
   i2c_master_setup();
   max518_d_a(0x00, 0, 0x80);	// constant on A/D0
   while(j)	// loop 5000 times
   {
      
      max518_d_a(0x00, 1, triangular[i]);

      ++i;           
      if (i>15)  
      {
         i=0;
      }
      --j;
      delay_ms(3);	// 3 ms * 15,000 = 15 seconds
   }

   max518_power_down(0);   // and then power down
   while(1)  /* endless loop */     ;
}

void max518_d_a(byte dev_adr, byte d_a, byte dat)
// dev_adr is  AD1, AD0 strapping, d_a is either 0 or 1, dat is data
// to be output
{
   i2c_master_start();
   i2c_master_out_byte(0x58 | (dev_adr<<1));	// address the device
   i2c_master_out_byte(d_a); 	// selects D/A
   i2c_master_out_byte(dat);	// d/a data
   i2c_master_stop();
}

void max518_power_down(byte dev_adr)
{
   i2c_master_start();
   i2c_master_out_byte(0x58 | (dev_adr<<1));	// address the device
   i2c_master_out_byte(0x08); 	// sets PD bit of command register
   i2c_master_stop();
}

#include <lcd_out.c>
#include <i2c_mstr.c>

// DS1803_1.BS2
//
// Illustrates how to control DS1803 Addressable Dual Potentiometer
//
// 16F877				DS1803
//
//  SCL/RC3 (term 18 ) --------- SCL (term 9) ----- To Other
//  SDA/RC4 (term 23) ---------- SDA (term 10) ----- I2C Devices
//
// Note that the slave address is determined by A2 (term 5), A1 (term 6)
// and A0 (term 7) on the 1803.  The above SCL and SDA leads may be multipled
// to eight devices, each strapped for a unique A2 A1 A0 setting.  In this
// example A2, A1 and A0 are strapped to ground.
//
// Pot 0 is set to a value of 0x40 (1/4) and Pot 1 to 0x80 (1/2).  The settings
// of the two pots are then read from the 1803 and displayed on the LCD.
//
// copyright Peter H. Anderson, Baltimore, MD, Mar, 01

#case

#device PIC16F877 *=16 ICD=TRUE

#include <defs_877.h>
#include <lcd_out.h>
#include <i2c_mstr.h>

#define TRUE !0
#define FALSE 0

void ds1803_write_pot(byte device, byte pot, byte setting);
void ds1803_read_pots(byte device, byte *p_setting_0, byte *p_setting_1);

void main(void)
{
   byte pot_setting_0, pot_setting_1;

   lcd_init();
   i2c_master_setup();

   ds1803_write_pot(0, 0, 0x40);  // dev 0, pot 0, setting 0x40 (1/4 of full)
   ds1803_write_pot(0, 1, 0x80);  // pot 1 setting 0x80 (1/2 of full)

   ds1803_read_pots(0, &pot_setting_0, &pot_setting_1);

   lcd_clr_line(0);
   printf(lcd_char, "POT 0 = ");
   lcd_hex_byte(pot_setting_0);
   lcd_clr_line(1);
   printf(lcd_char, "POT 1 = ");
   lcd_hex_byte(pot_setting_1);

   while(1)		;
}

void ds1803_write_pot(byte device, byte pot, byte setting)
//writes specified setting to specified potentiometer on specified device
{
   i2c_master_start();
   i2c_master_out_byte(0x50 | (device << 1));
   i2c_master_out_byte(0xa9 + pot);  // 0xa9 for pot 0, 0xaa for pot 1
   i2c_master_out_byte(setting);
   i2c_master_stop();
}

void ds1803_read_pots(byte device, byte *p_setting_0, byte *p_setting_1)
//reads data from both potentiometers
{
   i2c_master_start();
   i2c_master_out_byte(0x51 | (device << 1));
   *p_setting_0 = i2c_master_in_byte(TRUE);
   *p_setting_1 = i2c_master_in_byte(FALSE);
   i2c_master_stop();
}

#include <lcd_out.c>
#include <i2c_mstr.c>

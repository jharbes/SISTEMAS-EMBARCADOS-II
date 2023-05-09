// 1820_3.C
//
// Cyclic redudancy check (CRC).
//
// Performs temperature measurement and displays the nine values on serial LCD.
//
// Then calculates and displays CRC.  Note that the CRC of the 9 bytes should be
// zero.
//
// copyright, Peter H. Anderson, Georgetown, SC, Mar, '01

#case

#device PIC16F877 *=16 ICD=TRUE

#include <defs_877.h>
#include <lcd_out.h>
#include <_1_wire.h>

#define FALSE 0
#define TRUE !0

byte calc_crc(byte *buff, byte num_vals);

void main(void)
{
   byte buff[9], sensor=0, crc, n;

   while(1)
   {
      _1w_init(sensor);
      _1w_out_byte(sensor, 0xcc);  // skip ROM

      _1w_out_byte(sensor, 0x44);  // perform temperature conversion
      _1w_strong_pull_up(sensor);

      _1w_init(sensor);
      _1w_out_byte(sensor, 0xcc);  // skip ROM

      _1w_out_byte(sensor, 0xbe);

      for (n=0; n<9; n++)
      {
         buff[n]=_1w_in_byte(sensor);
      }

      lcd_init();

      for (n=0; n<4; n++)
      {
         lcd_hex_byte(buff[n]);
         lcd_char(' ');
      }

      lcd_clr_line(1);

      for (n=4; n<9; n++)
      {
         lcd_hex_byte(buff[n]);
         lcd_char(' ');
      }

      lcd_clr_line(2);

      crc = calc_crc(buff, 9);
      lcd_hex_byte(crc);

      delay_ms(2000);
   }
}

byte calc_crc(byte *buff, byte num_vals)
{
   byte shift_reg=0, data_bit, sr_lsb, fb_bit, i, j;

   for (i=0; i<num_vals; i++)	// for each byte
   {
      for(j=0; j<8; j++)	// for each bit
      {
         data_bit = (buff[i]>>j)&0x01;
         sr_lsb = shift_reg & 0x01;
         fb_bit = (data_bit ^ sr_lsb) & 0x01;
         shift_reg = shift_reg >> 1;
         if (fb_bit)
         {
            shift_reg = shift_reg ^ 0x8c;
         }
      }
   }
   return(shift_reg);
}

#include <lcd_out.c>
#include <_1_wire.c>

// 24_256_2.C
//
// Interface with 24LC256 using MSSP Module in I2C Master Mode
//
//
// copyright, Peter H. Anderson, Baltimore, MD, Jan, '01

#case

#device PIC16F877 *=16 ICD=TRUE

#include <defs_877.h>
#include <lcd_out.h>
#include <i2c_mstr.h>

#define TRUE !0
#define FALSE 0

void random_write(byte dev_adr, int mem_adr, byte dat);
byte random_read(byte dev_adr, int mem_adr);

// routines used for 24LC256
void make_meas_seq(byte *d);
void seq_write(byte dev_adr, unsigned long mem_adr, byte *d, byte num_vals);
void seq_read(byte dev_adr, unsigned long mem_adr, byte *d, byte num_vals);

void main(void)
{
   long mem_adr;
   byte dat, m, n, line;
   byte d[4];

   i2c_master_setup();

   lcd_init();
   printf(lcd_char, "Byte Demo");
   delay_ms(2000);

   lcd_clr_line(0);
   mem_adr=0x0700;
   for(n=0; n<16; n++)	// write some data to the EEPROM
   {
      dat = 0xff - n;
      random_write(0x00, mem_adr, dat);
      ++mem_adr;
      lcd_char('!');		// to show something is going on
   }

   // now, read the data back and display
   lcd_init();

   mem_adr=0x0700;
   for(n=0, m=0, line = 0; n<16; n++, m++)
   {
      if (m==4)
  	  {
	 	  m = 0;
		  ++line;
		  lcd_clr_line(line);
	  }
      dat = random_read(0x00, mem_adr);
      lcd_hex_byte(dat);
      lcd_char(' ');
      ++mem_adr;
   }
   delay_ms(2000);
   lcd_init();
   printf(lcd_char, "Seq Byte Demo");
   delay_ms(2000);

   lcd_init();

   mem_adr = 0x0700;		// write the data
   for(n=0; n<3; n++)		// three chuncks of 4 data bytes
   {
       make_meas_seq(d);
       seq_write(0x00, mem_adr, d, 4);
       mem_adr +=4;
   }

   mem_adr=0x0700;			// now read it back

   for(n=0; n<3; n++)
   {
      seq_read(0x00, mem_adr, d, 4);
      lcd_clr_line(n);
      for(m=0; m<4; m++)
      {
         lcd_hex_byte(d[m]);
         lcd_char(' ');
      }
      mem_adr +=4;
   }

   while(1)			;
}

void random_write(byte dev_adr, int mem_adr, byte dat)
{
   i2c_master_start();
   i2c_master_out_byte(0xa0 | (dev_adr << 1));
   i2c_master_out_byte((mem_adr >> 8) & 0xff);	// high byte of memory address
   i2c_master_out_byte(mem_adr & 0xff);	// low byte of mem address
   i2c_master_out_byte(dat);			// and finally the data
   i2c_master_stop();
   delay_ms(25); // allow for the programming of the eeprom
}

byte random_read(byte dev_adr, int mem_adr)
{
   byte y;
   i2c_master_start();
   i2c_master_out_byte(0xa0 | (dev_adr << 1));
   i2c_master_out_byte((mem_adr >> 8) & 0xff);
   i2c_master_out_byte(mem_adr & 0xff);

   i2c_master_repeated_start();	// no intermediate stop
   i2c_master_out_byte(0xa1 | (dev_adr << 1));	// read operation
   y=i2c_master_in_byte(1);
   i2c_master_stop();
   return(y);
}

void make_meas_seq(byte *d)
{
   static byte n = 0;
   d[0]=0xf0+n;
   d[1]=0xa0+n;
   d[2]=0x80+n;
   d[3]=0x40+n;
   n+=2;
}

void seq_write(byte dev_adr, unsigned long mem_adr, byte *d, byte num_vals)
{
   byte n;
   i2c_master_start();
   i2c_master_out_byte(0xa0 | (dev_adr << 1));
   i2c_master_out_byte((mem_adr >> 8) & 0xff);
   i2c_master_out_byte(mem_adr & 0xff);
   for (n=0; n<num_vals; n++)
   {
      i2c_master_out_byte(d[n]);
   }
   i2c_master_stop();
   delay_ms(10);
}

void seq_read(byte dev_adr, unsigned long mem_adr, byte *d, byte num_vals)
{
   byte n;

   i2c_master_start();
   i2c_master_out_byte(0xa0 | (dev_adr << 1));
   i2c_master_out_byte((mem_adr >> 8) & 0xff);
   i2c_master_out_byte(mem_adr & 0xff);

   i2c_master_repeated_start();
   i2c_master_out_byte(0xa1 | (dev_adr << 1));

   for(n=0; n<num_vals; n++)
   {
      if(n!=(num_vals-1))
      {
		 d[n] = i2c_master_in_byte(1);
      }
      else
      {
		 d[n] = i2c_master_in_byte(0);
	  }
   }
   i2c_master_stop();
}


#include <lcd_out.c>
#include <i2c_mstr.c>

// Program 24_256_1.C
//
// Illustrates how to write a byte to an address and read a byte from an
// an address.  The I2C interface is implemented using "bit bang" routines.
//
// Program writes the 16 values 0xff, 0xfe, etc to locations beginning
// at memory adr 0x0700.  Reads them back and displays on LCD.
//
// Then illustrates sequential write and read.
//
//    PIC16F877                     24LC256
//
//    RB1 (term 33)---------------- SCL (term 6) ----- To Other
//    RB2 (term 34) --------------- SDA (term 5) ----- I2C Devices
//
// Note that the slave address is determined by A2 (term 3), A1
// (term 2) and A0 (term 1) on the 24LC256.  The above SCL and SDA leads
// may be multipled to eight group "1010" devices, each strapped for a
// unique A2 A1 A0 setting.
//
// 4.7K pullup resistors to +5VDC are required on both signal leads.
//
// copyright, Peter H. Anderson, Baltimore, MD, Feb, '01

#case

#device PIC16F877 *=16 ICD=TRUE

#include <defs_877.h>
#include <lcd_out.h>
#include <i2c_bb.h>

#define TRUE !0
#define FALSE 0

#define SDA_DIR trisb2
#define SCL_DIR trisb1

#define SDA_PIN rb2
#define SCL_PIN rb1

// routines used for 24LC256 byte write and read
void random_write(byte dev_adr, unsigned long mem_adr, byte dat);
byte random_read(byte dev_adr, unsigned long mem_adr);

// routines used for 24LC256 seq byte write and read
void make_meas_seq(byte *d);
void seq_write(byte dev_adr, unsigned long mem_adr, byte *d, byte num_vals);
void seq_read(byte dev_adr, unsigned long mem_adr, byte *d, byte num_vals);

void main(void)
{
   unsigned int mem_adr;
   byte dat, m, n, line;
   byte d[4];

   lcd_init();

   i2c_setup_bb();

   // illustrates byte write and byte read
   printf(lcd_char, "Byte Write Demo");
   delay_ms(2000);

   lcd_init();

   mem_adr=0x0700;
   for(n=0; n<16; n++)
   {
	  lcd_char('!');		// to indicate something is going on
      dat = 0xff-n;
      random_write(0x00, mem_adr, dat);
      ++mem_adr;
   }

   line = 0;
   lcd_clr_line(line);
   mem_adr=0x0700;

   for(n=0, m=0; n<16; n++, m++)
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

   // illustrates sequential write and read
   lcd_init();
   printf(lcd_char, "Seq Byte Demo");
   delay_ms(2000);

   mem_adr = 0x0700;			// write the data
   for(n=0; n<3; n++)		// three chuncks of 4 data bytes
   {
       make_meas_seq(d);
       seq_write(0x00, mem_adr, d, 4);
       mem_adr +=4;
       lcd_char('!');		// to show something is happening
   }

   delay_ms(1000);

   lcd_init();

   mem_adr=0x0700;			// now read it back
   for(n=0; n<3; n++)
   {
       seq_read(0x00, mem_adr, d, 4);

       lcd_clr_line(n);		// and display it on the LCD
       for(m=0; m<4; m++)
       {
           lcd_hex_byte(d[m]);
           lcd_char(' ');
       }
       mem_adr +=4;
    }

   while(1)  /* continually loop */  ;
}

void random_write(byte dev_adr, unsigned long mem_adr, byte dat)
{
   i2c_start_bb();
   i2c_out_byte_bb(0xa0 | (dev_adr << 1));
   i2c_out_byte_bb((mem_adr >> 8) & 0xff);
   i2c_out_byte_bb(mem_adr & 0xff);
   i2c_out_byte_bb(dat);
   i2c_stop_bb();
   delay_ms(25); // allow for the programming of the eeprom
}

byte random_read(byte dev_adr, unsigned long mem_adr)
{
   byte y;
   i2c_start_bb();
   i2c_out_byte_bb(0xa0 | (dev_adr << 1));
   i2c_out_byte_bb((mem_adr >> 8) & 0xff);
   i2c_out_byte_bb(mem_adr & 0xff);

   i2c_start_bb();
   i2c_out_byte_bb(0xa1 | (dev_adr << 1));
   y=i2c_in_byte_bb(FALSE);	// no ack prior to stop
   i2c_stop_bb();
   return(y);
}

void make_meas_seq(byte *d)	// generates four values on each call
{
   static byte n=0;
   d[0]=0xf0+n;
   d[1]=0xa0+n;
   d[2]=0x80+n;
   d[3]=0x40+n;
   n+=2;
}

void seq_write(byte dev_adr, unsigned long mem_adr, byte *d, byte num_vals)
{
   byte n;
   i2c_start_bb();
   i2c_out_byte_bb(0xa0 | (dev_adr << 1));
   i2c_out_byte_bb((mem_adr >> 8) & 0xff);
   i2c_out_byte_bb(mem_adr & 0xff);

   for (n=0; n<num_vals; n++)
   {
      i2c_out_byte_bb(d[n]);
   }
   i2c_stop_bb();
   delay_ms(25);
}

void seq_read(byte dev_adr, unsigned long mem_adr, byte *d, byte num_vals)
{
   byte n;

   i2c_start_bb();
   i2c_out_byte_bb(0xa0 | (dev_adr << 1));
   i2c_out_byte_bb((mem_adr >> 8) & 0xff);
   i2c_out_byte_bb(mem_adr & 0xff);

   i2c_start_bb();			// repeated start
   i2c_out_byte_bb(0xa1 | (dev_adr << 1));

   for(n=0; n<num_vals; n++)
   {
      if(n!=(num_vals-1))
      {
		 d[n] = i2c_in_byte_bb(TRUE);	// ack after each byte
      }
      else
      {
		 d[n] = i2c_in_byte_bb(FALSE);	// its the last byte
	  }

   }
   i2c_stop_bb();
}

#include <lcd_out.c>
#include <i2c_bb.c>

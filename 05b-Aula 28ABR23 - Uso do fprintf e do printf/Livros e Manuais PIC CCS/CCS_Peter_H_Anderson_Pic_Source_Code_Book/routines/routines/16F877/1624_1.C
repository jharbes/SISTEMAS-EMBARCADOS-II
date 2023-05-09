// 1624_1.C
//
//    PIC16F877                   DS1624
//
//    SCL (term 18) ----------- SCL (term 2) ----- To Other
//    SDA (term 23) ----------- SDA (term 1) ----- I2C Devices
//
//
// Performs 10 temperature measurements and displays each result on the
// LCD and also save the data to the DS1624's EEPROM.  The data is then
// read from EEPROM and displayed on the LCD.
//
// copyright, Peter H. Anderson, Baltimore, MD, Mar, '01

#case

#device PIC16F877 *=16 ICD=TRUE

#include <defs_877.h>
#include <lcd_out.h>
#include <i2c_mstr.h>

#define TRUE !0
#define FALSE 0

// functions in this program
void ds1624_config(byte dev_adr, byte mode);
void ds1624_start_conv(byte dev_adr);
void ds1624_meas_temp(byte dev_adr, byte *p_whole, byte *p_fract);
byte ds1624_compute_fraction(byte t_fract);

void ds1624_ee_write(byte dev_adr, byte ee_adr,  byte dat);
byte ds1624_ee_read(byte dev_adr, byte ee_adr);

#define NUM_SAMPS 10

void main(void)
{
   byte m, n, line, ee_adr, dat_h, dat_l, temp_h, temp_l, t_whole, t_fract;

   lcd_init();
   i2c_master_setup();

   ds1624_config(0x02, 0x01);  // dev adr is 0x02, mode is 0x01 - 1 shot

   for(n = 0, ee_adr=0; n<NUM_SAMPS; n++)
   {
     ds1624_start_conv(0x02);
     delay_ms(1000);
     ds1624_meas_temp(0x02, &dat_h, &dat_l);
     		// note that pointers are passed
     if(dat_h & 0x80)		// result is negative
     {
	    temp_l = (~dat_l & 0xf8) + 0x08;
        temp_h = ~dat_h;
        if (temp_l == 0)
        {
          ++temp_h;
        }
	    lcd_char('-');
     }

  	 else
	 {
		 temp_h = dat_h;
		 temp_l = dat_l;
	 }
	 t_whole = temp_h;
     t_fract = ds1624_compute_fraction(temp_l);
	                          // convert to decimal format
     lcd_clr_line(0);
     lcd_dec_byte(t_whole, 2);
     lcd_char('.');	// decimal point
     lcd_dec_byte(t_fract, 2);

     ds1624_ee_write(0x02, ee_adr, dat_h);
     ds1624_ee_write(0x02, ee_adr+1, dat_l);
     ee_adr += 2;
     delay_ms(1000);
   }

   lcd_init();

   for (ee_adr = 0, n = 0, m = 0, line = 0; n<NUM_SAMPS; n++)
            // now fetch each from EEPROM and display
   {

      if (m==2)
      {
	     m=0;
	     ++line;
	     if (line == 4)
	     {
	        line = 0;
            lcd_init();
	     }
	     lcd_clr_line(line);
      }
      ++m;

      dat_h = ds1624_ee_read(0x02, ee_adr);
      dat_l = ds1624_ee_read(0x02, ee_adr+1);
      ee_adr += 2;

      if(dat_h & 0x80)		// result is negative
      {
	    temp_l = (~dat_l & 0xf8) + 0x08;
        temp_h = ~dat_h;
        if (temp_l == 0)
        {
          ++temp_h;
        }
	    lcd_char('-');
      }

	  else
	  {
	 	 temp_h = dat_h;
         temp_l = dat_l;
	  }

	  t_whole = temp_h;
      t_fract = ds1624_compute_fraction(temp_l);
	                          // convert to decimal format
      lcd_dec_byte(t_whole, 2);
      lcd_char('.');	// decimal point
      lcd_dec_byte(t_fract, 2);

      lcd_char(' ');
      delay_ms(500);
   }

   while(1)	/* loop */		;
}

void ds1624_config(byte dev_adr, byte mode)
// configures DS1624 in 1SHOT temperature conversion mode
{
   i2c_master_start();
   i2c_master_out_byte(0x90 | (dev_adr << 1));
   i2c_master_out_byte(0xac);	// access configuration
   i2c_master_out_byte(mode);
   i2c_master_stop();
   delay_ms(25);  	// wait for EEPROM to program
}

void ds1624_start_conv(byte dev_adr)
{
   i2c_master_start();
   i2c_master_out_byte(0x90 | (dev_adr << 1));
   i2c_master_out_byte(0xee);	// start conversion
   i2c_master_stop();
}

void ds1624_meas_temp(byte dev_adr, byte *p_whole, byte *p_fract)
// fetches temperature result.  Values t_whole and t_fract returned using
// pointers
{
   i2c_master_start();
   i2c_master_out_byte(0x90 | (dev_adr << 1));
   i2c_master_out_byte(0xaa);	// fetch temperature

   i2c_master_repeated_start();   	// no intermediate stop
   i2c_master_out_byte(0x90 | (dev_adr << 1) | 0x01);
   *p_whole=i2c_master_in_byte(TRUE);	// value pointed to by p_whole
   *p_fract=i2c_master_in_byte(FALSE);
   i2c_master_stop();
}

byte ds1624_compute_fraction(byte t_fract)
// converts high five bits in t_fract to a number in the range of
// 0 - 100.
{
    byte sum = 0;
    byte y, n;
	const byte dec_vals[5] = {3, 6, 12, 25, 50};
				// 3 / 100, 6/100, 12/100, etc
	y = t_fract >> 3;		// fractional part is now in lowest five bits
	for (n=0; n<5; n++)
	{
	   if (y&0x01)
	   {
		   sum = sum + dec_vals[n];
	   }
	   y = y >> 1;
	}
	return(sum);
}

byte ds1624_ee_read(byte dev_adr, byte ee_adr)
// returns content location of location ee_adr in DS1624 EEPROM
{
   byte y;

   i2c_master_start();
   i2c_master_out_byte(0x90 | (dev_adr << 1));
   i2c_master_out_byte(0x17);		// access memory

   i2c_master_out_byte(ee_adr);	// the eeprom address

   i2c_master_repeated_start();			// no intermediate stop
   i2c_master_out_byte(0x90 | (dev_adr << 1) | 0x01);

   y = i2c_master_in_byte(FALSE);
   i2c_master_stop();

   return(y);
}

void ds1624_ee_write(byte dev_adr, byte ee_adr, byte dat)
// writes content of dat to specified address ee_adr
{
   i2c_master_start();
   i2c_master_out_byte(0x90 | (dev_adr << 1));
   i2c_master_out_byte(0x17);		// access memory

   i2c_master_out_byte(ee_adr);	// the eeprom address
   i2c_master_out_byte(dat);		// the eeprom data
   i2c_master_stop();
   delay_ms(25);  		// wait for eeprom to program
}

#include <lcd_out.c>
#include <i2c_mstr.c>

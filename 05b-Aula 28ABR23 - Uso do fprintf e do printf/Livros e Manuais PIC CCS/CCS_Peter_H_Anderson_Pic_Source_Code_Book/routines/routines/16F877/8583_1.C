// 8583_1.C
//
// Writes a base time and date to clock.  Reads clock about every
// second and displays on LCD.
//
// PIC16F877							PCF8583
//
//  SCL (term 18) ------------------- SCL (term 6) ----- To Other
//  SDA (term 23) ------------------- SDA (term 5) ----- I2C Devices
//
// I2C address is 0xa0 or 0xa2 depending on strapping of A0 (terminal 3)
// In this example, A0 is at ground.
//
// copyright, Peter H. Anderson, Baltimore, MD, Mar, '01

#case

#device PIC16F877 *=16 ICD=TRUE

#include <defs_877.h>
#include <lcd_out.h>
#include <i2c_mstr.h>

#define TRUE !0
#define FALSE 0

struct Time
{
   byte hr;
   byte mi;
   byte se;
};

struct Date
{
   byte yr;
   byte mo;
   byte da;
   byte weekday;
};

// routines used for PCF8583
void _8583_configure_control_register(byte control_reg);
void _8583_write_clock(struct Time *p_t, struct Date *p_d);
void _8583_read_clock(struct Time *p_t, struct Date *p_d);

void _8583_display_date_time(struct Time *p_t, struct Date *p_d);

byte to_BCD(byte natural_binary);
byte to_natural_binary(byte BCD);

void main(void)
{
   struct Time t_base, t;
   struct Date d_base, d;
   t_base.hr=23;  t_base.mi=59;  t_base.se=00;
   d_base.yr=3; d_base.mo=2; d_base.da=28; d_base.weekday=1;
   // Note that year is 0 through 3.  Thus, if the base year is 2000
   // this is Feb 28, 2003, Monday

   lcd_init();
   i2c_master_setup();

   _8583_configure_control_register(0x00);  // 32.768 kHz, no alarm
   _8583_write_clock(&t_base, &d_base);

   while(1)
   {
      _8583_read_clock(&t, &d);
      _8583_display_date_time(&t, &d);
      delay_ms(1000);
   }
}

void _8583_write_clock(struct Time *p_t, struct Date *p_d)
{
// Note that most sig bit of hours is 12/24 hour format
// 4 year is in bits 7 and 6 of 0x05.  Lower six bits are day in BCD
// Location 0x06.  Weeks day is in bits 7, 6, 5 and month in lower
// five bits.

   byte v;

   i2c_master_start();
   i2c_master_out_byte(0xa0);
   i2c_master_out_byte(0x01);	// address of first write
   i2c_master_out_byte(0x00);	// hundreths of a second
   v = to_BCD(p_t->se);
   i2c_master_out_byte(v);
   v = to_BCD(p_t->mi);
   i2c_master_out_byte(v);	//  location 0x03
   v = to_BCD(p_t->hr);
   i2c_master_out_byte(v);
       // 24 hour format - 0x80 for 12 hour time
   v = to_BCD(p_d->da);
   i2c_master_out_byte((p_d->yr << 6) | v);
                     // YY TT UUUU
   v = to_BCD(p_d->mo);
   i2c_master_out_byte((p_d->weekday << 5) | v );
                    // DDD T UUUU
   i2c_master_stop();
}

void _8583_read_clock(struct Time *p_t, struct Date *p_d)
{
   byte v;
   i2c_master_start();
   i2c_master_out_byte(0xa0);
   i2c_master_out_byte(0x02);	// begin with seconds

   i2c_master_repeated_start();
   i2c_master_out_byte(0xa1);

   v = i2c_master_in_byte(TRUE) & 0x7f;
   p_t->se = to_natural_binary(v);
   v = i2c_master_in_byte(TRUE);
   p_t->mi = to_natural_binary(v);
   v = i2c_master_in_byte(TRUE) & 0x3f;
   p_t->hr = to_natural_binary(v);

   v = i2c_master_in_byte(TRUE);
   p_d->yr = v >> 6;		// year is in two most sig bits
   v = v & 0x3f;	// day in lower six bits
   p_d->da = to_natural_binary(v);

   v = i2c_master_in_byte(FALSE);
   p_d->weekday = v >> 5;
   v = v & 0x1f;
   p_d->mo = to_natural_binary(v);
   // no ack prior to stop
   i2c_master_stop();
}

void _8583_configure_control_register(byte control_reg)
{
   i2c_master_start();
   i2c_master_out_byte(0xa0);
   i2c_master_out_byte(0x00);  // control register address
   i2c_master_out_byte(control_reg);
   i2c_master_stop();
}

void _8583_display_date_time(struct Time *p_t, struct Date *p_d)
{
  lcd_init();
  lcd_dec_byte(p_d->yr + 00, 2); // assumes base year of 2000
  lcd_char('/');
  lcd_dec_byte(p_d->mo, 2);
  lcd_char('/');
  lcd_dec_byte(p_d->da, 2);
  lcd_char(' ');
  lcd_dec_byte(p_t->hr, 2);
  lcd_char(':');
  lcd_dec_byte(p_t->mi, 2);
  lcd_char(':');
  lcd_dec_byte(p_t->se, 2);

  lcd_clr_line(1);
  lcd_dec_byte(p_d->weekday, 1);
}

byte to_BCD(byte natural_binary)
{
   return ( ((natural_binary/10) << 4) + natural_binary%10 );
}

byte to_natural_binary(byte BCD)
{
   return(  ((BCD >> 4) * 10) + (BCD & 0x0f)  );
}

#include <lcd_out.c>
#include <i2c_mstr.c>


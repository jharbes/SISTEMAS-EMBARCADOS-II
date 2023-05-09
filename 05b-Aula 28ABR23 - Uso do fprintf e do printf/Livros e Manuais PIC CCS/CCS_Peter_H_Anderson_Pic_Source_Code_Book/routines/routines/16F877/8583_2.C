// 8583_2.C
//
// Illustrates how to force a periodic interrupt using the PCF8583s
// alarm function.
//
// Sets clock to a base date and time.  Sets alarm to two minutes later.
// Configures for dated alarm.  Reads and displays the date and time.
//
// On interrupt, sets alarm for 2 minutes later, displays the date
// and time and momentarily flashes an LED on PORTD7
//
// PIC16F877            		PCF8583
//
//  SCL (term 18) ---------- SCL (term 6) ----- To Other
//  SDA (term 23) ---------- SDA (term 5) ----- I2C Devices
//
// I2C address is 0xa0 or 0xa2 depending on strapping of A0 (terminal 3)
// In this example, A0 is at ground.
//
//  PCF8583			    		 PIC16F877
//
//  /INT (term 7) ------------------- RB0
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
void _8583_configure_alarm_register(byte alarm_control_reg);

void _8583_write_clock(struct Time *p_t, struct Date *p_d);
void _8583_write_alarm(struct Time *p_t, struct Date *p_d);
void _8583_read_clock(struct Time *p_t, struct Date *p_d);
void _8583_read_alarm(struct Time *p_t, struct Date *p_d);

void _8583_display_date_time(struct Time *p_t, struct Date *p_d);

void calc_new_time(struct Time *p_t, struct Date *p_d, unsigned long minutes);
void increment_time_minutes(struct Time *p_t, struct Date *p_d);
void increment_time_hours(struct Time *p_t, struct Date *p_d);
void increment_date(struct Date *p_date);

byte to_BCD(byte natural_binary);
byte to_natural_binary(byte BCD);

#define LED_DIR trisd7
#define LED_PIN portd7

byte const days_in_month[13] = {0, 31, 28, 31, 30, 31, 30,
                                  31, 31, 30, 31, 30, 31};
// Note that Jan is month 1.  Element 0 is not used

byte ext_int_occurred;		// global variable to indicate an interrupt occurred

void main(void)
{
   struct Time t;
   // note these numbers are in natural binary
   struct Date d;
   t.hr=23;  t.mi=59;  t.se=00;
   d.yr=3; d.mo=2; d.da=28; d.weekday=1;

   lcd_init();
   i2c_master_setup();

   not_rbpu = 0;	// enable internal pullups on PORTB

   pspmode = 0;		// use PORTD as general purpose IO
   LED_DIR = 0;
   LED_PIN = 0;   // turn off LED

   ext_int_occurred = FALSE;
   _8583_write_clock(&t, &d);
   _8583_display_date_time(&t, &d);		
   delay_ms(1000);

   calc_new_time(&t, &d, 2);			// two minutes
   _8583_display_date_time(&t, &d);		// to verify calculation is working
   _8583_write_alarm(&t, &d);
   _8583_configure_alarm_register(0x80 | 0x20 | 0x10); // dated alarm
   _8583_configure_control_register(0x04); // 32.768 kHz, alarm
   delay_ms(1000);

   _8583_read_clock(&t, &d);  	// read and display the time
   _8583_display_date_time(&t, &d);
   delay_ms(1000);

   _8583_read_alarm(&t, &d);	// read an display the alarm value
   _8583_display_date_time(&t, &d);
   delay_ms(1000);

   intf = 0;	// kill any pending interrupts
   intedg = 0;	// negative edge
   inte = 1;
   gie = 1;		// enable interrupts

   while(1)
   {
      if(ext_int_occurred)
      {
         while (gie)	// for the moment disable interrupts
         {
            gie = 0;
         }
         ext_int_occurred = FALSE;
         _8583_read_alarm(&t, &d);
         calc_new_time(&t, &d, 2);	// add two minutes
         _8583_write_alarm(&t, &d);
         _8583_configure_alarm_register(0x80 | 0x20 | 0x10); // dated alarm
         _8583_configure_control_register(0x04); // 32.768 kHz, alarm

         _8583_read_alarm(&t, &d);  // read and display the new alarm
         _8583_display_date_time(&t, &d);
         LED_PIN = 1;   // momentarily wink the LED
         delay_ms(1000);
         LED_PIN = 0;
         gie = 1;	// enable interrupts again
      }
   }
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

void calc_new_time(struct Time *p_t, struct Date *p_d, unsigned long minutes)
{
   byte n, hours, mins;
   hours = (byte)(minutes / 60);	// split into number of hours and remaining minutes
   mins = (byte)(minutes % 60);

   for (n=0; n<hours; n++)
   {
	   increment_time_hours(p_t, p_d);
   }

   for (n=0; n<mins; n++)
   {
	   increment_time_minutes(p_t, p_d);
   }
}

void increment_time_minutes(struct Time *p_t, struct Date *p_d)
{
	++p_t->mi;
	if (p_t->mi > 59)
	{
		p_t->mi = 0;
		increment_time_hours(p_t, p_d);
	}
}

void increment_time_hours(struct Time *p_t, struct Date *p_d)
{
	++p_t->hr;
	if (p_t->hr > 23)
	{
		p_t->hr = 0;
		increment_date(p_d);
	}
}

void increment_date(struct Date *p_date)
{
   if ((p_date->da) == days_in_month[p_date->mo])
	// currently last day of the month
   {
      if(  (p_date->yr==0) && (p_date->mo == 2)  ) // leap year
      {
         ++p_date->da;	// Feb 29
      }
      else if (p_date->mo == 12)	// Dec 31
      {
          ++p_date->yr;
          if (p_date->yr == 4)
          {
             p_date->yr=0;	// roll over the century
          }
          p_date->mo=1;
          p_date->da=1;		// Jan 1
      }
      else
      {
        ++p_date->mo;		// set to first day of new month
        p_date->da=1;
      }
   }
   else // not the last day of the month
   {
      ++p_date->da;  // simply increment the date
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
   i2c_master_out_byte(v);	// 0x03

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

void _8583_write_alarm(struct Time *p_t, struct Date *p_d)
{
// Note that most sig bit of hours is 12/24 hour format
// 4 year is in bits 7 and 6 of 0x05.  Lower six bits are day in BCD
// Location 0x06.  Weeks day is in bits 7, 6, 5 and month in lower
// five bits.
//
// Note that this function might have been combined with
// _8583_write_clock by passing an indication as to whether clock
// or alarm.

   byte v;

   i2c_master_start();
   i2c_master_out_byte(0xa0);
   i2c_master_out_byte(0x01+8);// address of first write
   i2c_master_out_byte(0x00);	// hundreths of a second

   v = to_BCD(p_t->se);
   i2c_master_out_byte(v);

   v = to_BCD(p_t->mi);
   i2c_master_out_byte(v);	// 0x03 + 8

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
   // no ack
   i2c_master_stop();
}

void _8583_read_alarm(struct Time *p_t, struct Date *p_d)
{
   byte v;
   i2c_master_start();
   i2c_master_out_byte(0xa0);
   i2c_master_out_byte(0x02 + 8);	// begin with alarm seconds

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
   // no ack
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

void _8583_configure_alarm_register(byte alarm_control_reg)
{
   i2c_master_start();
   i2c_master_out_byte(0xa0);
   i2c_master_out_byte(0x08);  // control register address
   i2c_master_out_byte(alarm_control_reg);
   i2c_master_stop();
}

byte to_BCD(byte natural_binary)
{
   return ( ((natural_binary/10) << 4) + natural_binary%10 );
}

byte to_natural_binary(byte BCD)
{
   return(  ((BCD >> 4) * 10) + (BCD & 0x0f)  );
}

#int_ext ext_int_handler(void)
{
    ext_int_occurred = TRUE;   // flag that there has been an interrupt
}

#int_default default_int_handler(void)
{
}

#include <lcd_out.c>
#include <i2c_mstr.c>

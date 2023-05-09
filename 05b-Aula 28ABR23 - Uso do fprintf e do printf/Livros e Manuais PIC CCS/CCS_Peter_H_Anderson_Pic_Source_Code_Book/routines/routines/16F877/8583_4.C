
// 8583_4.C
//
// Illustrates use of the PCF8583 in a counting mode.  Use of the
// counting alarm (interrupt on match) is also illustrated.
//
// Counter is set to 00 00 00 and alarm to 00 00 50.  On interrupt
// alarm is advanced by 50 and an LED on PORTD is winked, and this
// process continues.  The 10 pps output of the Morgan Logic Probe
// (term 2) may be used as a counter source.
//
// Note that the counter and alarm values are each stored as three
// two digit BCD values in a structure.  Calculation of a new alarm
// value is implemented by manipulating this structure.
//
// This program closely follows program 8583_2.C except this program
// counts events rather than performing a timing function.
//
// PIC16F877			      PCF8583
//
//  SCL (term 18) ----------- SCL (term 6) ----- To Other
//  SDA (term 23)------------ SDA (term 5) ----- I2C Devices
//
// I2C address is 0xa0 or 0xa2 depending on strapping of A0 (terminal
// 3) In this example, A0 is at logic zero.
//
// PCF8583			   		   PIC16F877
//
// /INT (term 7) ------------- RB0 (term 33)
//
// copyright, Peter H. Anderson, Baltimore, MD, Mar, '01

#case

#device PIC16F877 *=16 ICD=TRUE

#include <defs_877.h>
#include <lcd_out.h>
#include <i2c_mstr.h>

#define TRUE !0
#define FALSE 0

struct BCD3
{
   byte tt;
   byte hu;
   byte un;
};

// routines used for PCF8583
void _8583_configure_control_register(byte control_reg);
void _8583_configure_alarm_register(byte alarm_control_reg);

void _8583_write_event_count(struct BCD3 *p);
void _8583_write_event_alarm(struct BCD3 *p);
void _8583_read_event_count(struct BCD3 *p);
void _8583_read_event_alarm(struct BCD3 *p);

void _8583_display_event_count(struct BCD3 *p);

void calc_new_BCD3(struct BCD3 *p, long num_events);
byte increment_BCD(byte x);

byte to_BCD(byte natural_binary);
byte to_natural_binary(byte BCD);

#define LED_DIR trisd7
#define LED_PIN portd7

byte ext_int_occurred;

void main(void)
{
   struct BCD3 count;
   byte line = 0;
   // note these numbers are in BCD
   count.tt=0x00;  count.hu=0x00;  count.un=0x00;

   lcd_init();
   i2c_master_setup();

   not_rbpu = 0;
   pspmode = 0;

   LED_DIR = 0;
   LED_PIN = 0;   // turn off LED

   ext_int_occurred = FALSE;
   _8583_write_event_count(&count);			// zero the counter
   calc_new_BCD3(&count, 50);
   lcd_clr_line(0);
   _8583_display_event_count(&count);		// a check that the calc_new_BCD3
   											// is working

   _8583_write_event_alarm(&count);
   _8583_configure_alarm_register(0x80 | 0x10); // event alarm
   _8583_configure_control_register(0x24); // event counter, alarm

   _8583_read_event_count(&count);  		// read and display the count

   lcd_clr_line(1);
   _8583_display_event_count(&count);		// to verify circuit is working

   delay_ms(1000);

   intf = 0;	// kill any pending interrupts
   intedg = 0;	// negative edge
   inte = 1;
   gie = 1;

   while(1)
   {
	  _8583_read_event_count(&count);  // continually read and
	  									//display the count
      lcd_clr_line(0);
	  printf(lcd_char, "Count  ");
      _8583_display_event_count(&count);
      delay_ms(50);
      if(ext_int_occurred)				// if an interrupt
      {
         while (gie)	// for the moment disable interrupts
         {
            gie = 0;
         }
         ext_int_occurred = FALSE;
         _8583_read_event_alarm(&count);	// read the alarm
         calc_new_BCD3(&count, 50);	// 50 more counts
         _8583_write_event_alarm(&count);	// new alarm value
         lcd_clr_line(1);
         printf(lcd_char, "Alarm ");
         _8583_display_event_count(&count);
         _8583_configure_alarm_register(0x80 | 0x10); // event alarm
         _8583_configure_control_register(0x24); // event, alarm
         LED_PIN = 1;   // momentarily wink the LED
         delay_ms(1000);
         LED_PIN = 0;
         gie = 1;	// enable interrupts again
      }
   }
}

void _8583_display_event_count(struct BCD3 *p)
{
  lcd_hex_byte(p->tt);	// note that these values are stored in BCD
  lcd_hex_byte(p->hu);
  lcd_hex_byte(p->un);
}

void _8583_write_event_count(struct BCD3 *p)
{

   i2c_master_start();
   i2c_master_out_byte(0xa0);
   i2c_master_out_byte(0x01);	// address of first write
   i2c_master_out_byte(p->un);
   i2c_master_out_byte(p->hu);
   i2c_master_out_byte(p->tt);
   i2c_master_stop();
}

void _8583_write_event_alarm(struct BCD3 *p)
{

   i2c_master_start();
   i2c_master_out_byte(0xa0);
   i2c_master_out_byte(0x09);	// address of first write
   i2c_master_out_byte(p->un);
   i2c_master_out_byte(p->hu);
   i2c_master_out_byte(p->tt);
   i2c_master_stop();
}


void _8583_read_event_count(struct BCD3 *p)
{
   i2c_master_start();
   i2c_master_out_byte(0xa0);
   i2c_master_out_byte(0x01);

   i2c_master_repeated_start();
   i2c_master_out_byte(0xa1);
   p->un = i2c_master_in_byte(TRUE);	// units
   p->hu = i2c_master_in_byte(TRUE);		// hundreds
   p->tt = i2c_master_in_byte(FALSE);		// ten thousands

   i2c_master_stop();
}

void _8583_read_event_alarm(struct BCD3 *p)
{
   i2c_master_start();
   i2c_master_out_byte(0xa0);
   i2c_master_out_byte(0x09);

   i2c_master_repeated_start();
   i2c_master_out_byte(0xa1);
   p->un = i2c_master_in_byte(TRUE);
   p->hu = i2c_master_in_byte(TRUE);
   p->tt = i2c_master_in_byte(FALSE);
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

void calc_new_BCD3(struct BCD3 *p, unsigned long num_events)
{
   byte tt, hu, un, n;

   tt = (byte) (num_events / 10000);
   num_events = num_events % 10000;
   hu = (byte) (num_events / 100);
   un = (byte) (num_events % 100);

   for (n = 0; n<tt; n++)
   {
	   p->tt = increment_BCD(p->tt);
   }

   for (n=0; n<hu; n++)
   {
	   p->hu = increment_BCD(p->hu);
	   if (p->hu == 0)
	   {
		   p->tt = increment_BCD(p->tt);
	   }
   }

   for (n=0; n<un; n++)
   {
	   p->un = increment_BCD(p->un);
	   if (p->un == 0)
	   {
		   p->hu = increment_BCD(p->hu);
		   if (p->hu == 0)
		   {
			  p->tt = increment_BCD(p->tt);
		   }
	   }
   }
}

byte increment_BCD(byte x)
{
	byte h, l;

	h = x / 16;
	l = x % 16;

	++l;
	if (l > 9)
	{
		l = 0;
		++h;
		if (h>9)
		{
			h = 0;
		}
	}
	return ((h * 16) + l);
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

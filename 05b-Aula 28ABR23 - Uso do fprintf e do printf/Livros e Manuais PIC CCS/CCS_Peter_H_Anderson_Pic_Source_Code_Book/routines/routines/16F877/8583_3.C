// 8583_3.C
//
// Illustrates use of the timer in location 0x07.
//
// Sets timout for 20 seconds.  On timeout, turns on LED and sets
// timeout for 10 secs.  On timeout turns LED off.  The LED is on PORTD7.
//
// PIC16F877			      PCF8583
//
//  SCL (term 18) ----------- SCL (term 6) ----- To Other
//  SDA (term 23)------------ SDA (term 5) ----- I2C Devices
//
// I2C address is 0xa0 or 0xa2 depending on strapping of A0 (terminal 3)
// In this example, A0 is at logic zero.
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

// routines used for PCF8583
void _8583_configure_control_register(byte control_reg);
  // 1 in bit 2 to enable alarm
void _8583_configure_alarm_register(byte alarm_control_reg);
  // 1 1 00  0  010
void _8583_zero_clock(void);
void _8583_set_timeout(byte seconds);
  // zero location 0x07 and set location 0x0f to timeout val

#define LED_DIR trisd7
#define LED_PIN portd7

int ext_int_occurred;	// global

void main(void)
{
   lcd_init();
   i2c_master_setup();

   pspmode = 0;
   not_rbpu = 0;

   ext_int_occurred = FALSE;	// defined globally

   LED_PIN = 0;
   LED_DIR = 0;

   _8583_zero_clock();
   // configure to timeout in 20 seconds
   _8583_set_timeout(0x20);	// note that this is BCD
   _8583_configure_alarm_register(0xc2);
   // alarm flag interrupt, timer alarm, seconds

   _8583_configure_control_register(0x04);	// enable alarm

   intf = 0;	// clear any interrupt
   intedg = 0;
   inte = 1;
   gie = 1;

   while(1)
   {

      if (ext_int_occurred)
      {
          while(gie)	// momentarily turn off interrupts
          {
             gie = 0;
          }
          ext_int_occurred = FALSE;

          if(!LED_PIN)	// if LED currently at zero
			// make it a one for 10 seconds
          {
             _8583_set_timeout(0x10);	// note that this is BCD
             LED_PIN = 1;
          }
          else
          {
             _8583_set_timeout(0x20);
             LED_PIN = 0;
          }
          _8583_configure_alarm_register(0xc2);
          // alarm flag interrupt, timer alarm, seconds
          // 0xc3 for minutes, 0xc4 for hours, 0x05 for days
          _8583_configure_control_register(0x04);	// enable alarm
               // note that this also clears alarm flag
          gie = 1;
      } // end of if

   }
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

void _8583_set_timeout(byte seconds)
  // zero location 0x07 and set location 0x0f to timeout val
{
   i2c_master_start();
   i2c_master_out_byte(0xa0);
   i2c_master_out_byte(0x07);  // timer
   i2c_master_out_byte(0x00);
   i2c_master_stop();

   i2c_master_start();
   i2c_master_out_byte(0xa0);
   i2c_master_out_byte(0x0f);  // timer alarm location
   i2c_master_out_byte(seconds);
   i2c_master_stop();
}

void _8583_zero_clock(void)
{
// set hours, mintues and secs to zero

   i2c_master_start();
   i2c_master_out_byte(0xa0);
   i2c_master_out_byte(0x01);	// address of first write
   i2c_master_out_byte(0x00);	// hundreths of a second
   i2c_master_out_byte(0x00);
   i2c_master_out_byte(0x00);	// 0x03
   i2c_master_out_byte(0x00);
      // 24 hour format - 0x80 for 12 hour time
   i2c_master_stop();
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

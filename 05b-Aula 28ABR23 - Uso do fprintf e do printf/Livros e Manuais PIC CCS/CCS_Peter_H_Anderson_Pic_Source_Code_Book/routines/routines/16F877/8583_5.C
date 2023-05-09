// 8583_5.C
//
// Similar to routine 8583_3.C except PCF8583 is configured to count
// events.
//
// Illustrates use of the "timer" in location 0x07.
//
// Sets "timout" for 20 events.  On interrupt, turns on LED on PORTD7
// and sets next interrupt for 10 events.  On interrupt, turns LED off
// and sets  the next interrupt for 20 events.  The Morgan Logic Probe
// provides nominal one and ten pps outputs which may be used as a
// counter source.
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

// routines used for PCF8583
void _8583_configure_control_register(byte control_reg);
  // 1 in bit 2 to enable alarm
void _8583_configure_alarm_register(byte alarm_control_reg);
  // 1 1 00  0  010
void _8583_zero_event_counter(void);
void _8583_set_event_trip(byte events);
  // zero location 0x07 and set location 0x0f to event trip

#define LED_DIR trisd7
#define LED_PIN portd7

int ext_int_occurred;	// global

void main(void)
{

   lcd_init();		// for possible debugging
   i2c_master_setup();

   pspmode = 0;
   not_rbpu = 0;

   LED_PIN = 0;
   LED_DIR = 0;

   ext_int_occurred = FALSE;	// defined globally

   _8583_zero_event_counter();
   // configure to timeout in 20 events
   _8583_set_event_trip(0x20);	// note that this is BCD
   _8583_configure_alarm_register(0xc1);
   // alarm flag interrupt, "timer" alarm, units

   _8583_configure_control_register(0x24);	// enable alarm, event counter

   intf = 0;	// clear any interrupt
   intedg = 0;	// 1 -> 0 causes interrupt
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

          if(!LED_PIN)	// if RB5 currently at zero
			// make it a one for 10 seconds
          {
             _8583_set_event_trip(0x10);	// note that this is BCD
             LED_PIN = 1;
          }
          else
          {
             _8583_set_event_trip(0x20);
             LED_PIN = 0;
          }
          _8583_configure_alarm_register(0xc1);
          // alarm flag interrupt, event mode, units
          // 0xc2 for hundreds, 0xc3 for 10,000, 0xc4 for millions
          _8583_configure_control_register(0x24);	// enable alarm
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

void _8583_set_event_trip(byte events)
  // zero location 0x07 and set location 0x0f to events
{
   i2c_master_start();
   i2c_master_out_byte(0xa0);
   i2c_master_out_byte(0x07);  // timer
   i2c_master_out_byte(0x00);
   i2c_master_stop();

   i2c_master_start();
   i2c_master_out_byte(0xa0);
   i2c_master_out_byte(0x0f);  // timer alarm location
   i2c_master_out_byte(events);
   i2c_master_stop();
}

void _8583_zero_event_counter(void)
{
// set units, hundres and 10,000 to zero

   i2c_master_start();
   i2c_master_out_byte(0xa0);
   i2c_master_out_byte(0x01);	// address of first write
   i2c_master_out_byte(0x00);	// units
   i2c_master_out_byte(0x00);	// hundreds
   i2c_master_out_byte(0x00);	// 10,000
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


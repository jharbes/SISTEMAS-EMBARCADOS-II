// DS2435_1.C (PIC16F877)
//
// Interface with Battery Monitor DS2435
//
// Reads and displays two byte ID.  Reads and diplays temperature.  Resets
// cycle counter and then increments counter five times, reads counter and
// displays.
//
// Sets sample rate to 0.5 minutes, clears elapsed time counter and then goes
// into loop, continually reading and displaying the elapsed time every ten
// seconds.
//
// PIC16F877							DS2435
//										+5 VDC --
// PORTD0 (term 19) ----------------------------- DQ
//										GRD -----
//
// Note that a 4.7K pullup resistor to +5 VDC is on the DQ lead.
//
// This was developed by Ernest N. Wells, Jr. as a part of his Senior Project
// at Morgan State University.
//
// copyright, Peter H. Anderson, Baltimore, MD, May, '01

#case
#device PIC16F877 *=16 ICD=TRUE
#include <a:\defs_877.h>
#include <a:\lcd_out.h>
#include <a:\_1_wire.h>

#define FALSE 0
#define TRUE !0

void read_2435_ID(byte *a);
void read_temperature(byte *a);

void reset_2435_cycle_counter(void);
void increment_2435_cycle_counter(void);
void read_2435_cycle_counter(byte *a);

void set_2435_sample_rate(byte v);
void set_2435_elapsed_time(byte *a);
void read_elapsed_time(byte *a);


void main(void)
{
   byte a[6], n;

   lcd_init();

   pspmode = 0;            // configure PORT as GPIO

   read_2435_ID(a);     // two byte result in array;
   printf(lcd_char, "ID = %2x%2x", a[1], a[0]);  // display ID

   read_temperature(a); // perform a temperature measurement
   lcd_clr_line(1);
   printf(lcd_char, "T_C = %3.1f  %d", (float) a[0]/2.0, a[1]);
     // display in the two formats

   reset_2435_cycle_counter();

   for (n=0; n<5; n++)
   {
     increment_2435_cycle_counter();
   }

   read_2435_cycle_counter(a); // fetch two byte value of cycle counter
   lcd_clr_line(2);
   printf(lcd_char, "Count = %2x%2x", a[1], a[0]);  // display cycles, high byte first

   set_2435_sample_rate(0x00); // set sample rate for 1/2 minute

   a[0] = 0x00; a[1] = 0x00;  a[2] = 0x00; // set clock to zero
   set_2435_elapsed_time(a);  // pass setting in first three bytes of array a

   while(1)    // continually read the elapsed time and display every 10 secs
   {
      read_elapsed_time(a);
      lcd_clr_line(3);
      printf(lcd_char, "ET = %u", a[0]); // display only the least sig byte

      _1w_init(0);  // release DS2435 to perform timing
      delay_ms(10000);
   }
}

void read_2435_ID(byte *a)
{
   _1w_init(0);       /* reset */
   _1w_out_byte(0,0xb2);   /* read registers on page 5 (ID bytes) */
   _1w_out_byte(0,0x80);   /* ID address 80h and 81h */
   a[0] = _1w_in_byte(0);
   a[1] = _1w_in_byte(0);
}

void read_temperature(byte *a)
{
   _1w_init(0);
   _1w_out_byte(0,0xd2);     // initiate temperature conversion cycle
    delay_ms(1000);

   _1w_init(0);
   _1w_out_byte(0,0xb2);      // read registers
   _1w_out_byte(0,0x60);

   a[0] = _1w_in_byte(0);     // 1/2 celceus temperature 0 - 127.5
   a[1] = _1w_in_byte(0);
}

void reset_2435_cycle_counter(void)
{
   _1w_init(0);
   _1w_out_byte(0,0xb8);
}
void increment_2435_cycle_counter(void)
{
   _1w_init(0);
   _1w_out_byte(0,0xb5);
}
void read_2435_cycle_counter(byte *a)
{
   _1w_init(0);
   _1w_out_byte(0,0xb2);      // read registers
   _1w_out_byte(0,0x82);

   a[0] = _1w_in_byte(0);     // low byte
   a[1] = _1w_in_byte(0);    // high byte
}

void set_2435_sample_rate(byte v)
{
   _1w_init(0);
   _1w_out_byte(0,0xef);      // write registers
   _1w_out_byte(0,0x8b);
   _1w_out_byte(0,v);
}

void set_2435_elapsed_time(byte *a)
{
   _1w_init(0);
   _1w_out_byte(0,0xe6);      // set clock
   _1w_out_byte(0,a[0]);
   _1w_out_byte(0,a[1]);
   _1w_out_byte(0,a[2]);
}

void read_elapsed_time(byte *a)
{
   _1w_init(0);
   _1w_out_byte(0,0xb2);      // read registers
   _1w_out_byte(0,0x74);

   a[0] = _1w_in_byte(0);     // low byte
   a[1] = _1w_in_byte(0);    // mid byte
   a[2] = _1w_in_byte(0);    // high byte
}

#include <a:\lcd_out.c>
#include <a:\_1_wire.c>

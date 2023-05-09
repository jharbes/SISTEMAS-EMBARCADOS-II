// DS2435_2.C (PIC16F877)
//
// Illustrates histogram feature of the ZDS2435.
//
// Clears the elapsed time counter and sets the sample rate for 0.5 minutes.
// Sets seven temperature boundaries, TA - TG, to define eight temperature bins.
// Clears the current histogram.
//
// The program then enters a continual loop.  The DS2435 is released (_1w_init)
// to perfrom its off line operation and after a 30 second delay, the histogram
// is read and displayed.
//
// PIC16F877								DS2435
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

void set_2435_temperature_bounds(byte *a);

void reset_2435_histogram(void);
void read_2435_histogram(byte *a);

void set_2435_sample_rate(byte v);
void set_2435_elapsed_time(byte *a);

void main(void)
{
    byte const temp_bounds[7] = {20, 21, 22, 23, 24, 25, 26};
    byte a[16], line, n;

    lcd_init();

    printf(lcd_char, "..............");

    a[0] = 0x00;  a[1] = 0x00;  a[2] = 0x00;
    set_2435_elapsed_time(a);

    set_2435_sample_rate(0x00);
    reset_2435_histogram();

    for (n=0; n<7; n++)    // seven boundaries
    {
       a[n] = temp_bounds[n];
    }
    set_2435_temperature_bounds(a);

    while(1)
    {
       _1w_init(0);
       delay_ms(30000); // 30 second delay

       read_2435_histogram(a);
       lcd_init();
       for (n=0, line = 0; n<16; n+=2)
       {
         if (((n%4) == 0) && (n!=0))
         {
           ++line;
           lcd_clr_line(line);
         }
         printf(lcd_char, "%2x%2x  ", a[n+1], a[n]);
      }
   }
}

void set_2435_temperature_bounds(byte *a)
{
    byte n;
    _1w_init(0);
    _1w_out_byte(0,0xef);  /* write register */
    _1w_out_byte(0,0x84);

    for (n=0; n<7; n++)
    {
       _1w_out_byte(0, a[n]);
    }
}

void reset_2435_histogram(void)
{
   _1w_init(0);
   _1w_out_byte(0,0xe1);
}

void read_2435_histogram(byte *a)
{
   byte n;
   _1w_init(0);   /* reset */
   _1w_out_byte(0,0xb2);     /* read registers */
   _1w_out_byte(0,0x64); /* starting address */

   for (n = 0; n < 16; n++)
   {
     a[n] = _1w_in_byte(0);
   }
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
   _1w_out_byte(0,0xe6);
   _1w_out_byte(0,a[0]);
   _1w_out_byte(0,a[1]);
   _1w_out_byte(0,a[2]);
}

#include <a:\lcd_out.c>
#include <a:\_1_wire.c>

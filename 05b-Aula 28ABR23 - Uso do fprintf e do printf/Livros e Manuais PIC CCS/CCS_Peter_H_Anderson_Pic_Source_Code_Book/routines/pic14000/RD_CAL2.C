// Rd_Cal2.C - PIC14000
//
// Reads calibration values from locations beginning at 0xfc0 and displays to
// the terminal.
//
// copyright, Peter H Anderson, Elmore, VT, June 2, '02

#case

#device PIC14000 *=16

#include <defs_14.h>
#include <delay.h>
#include <ser_14.h>

#define TRUE !0
#define FALSE 0

#define TxData 2	// PORTC, Bit 2
#define INV

#define CERDIP1

void read_calibration_float(unsigned long adr, byte *array, float *p_float);
byte read_calibration_byte(unsigned long adr);

void main(void)
{
	float Kref, Kbg, Vtherm, Ktc;	// calibration constants
	byte Fosc, Twdt;

	byte a[4], n;

    while(1)
    {

       ser_init();
       lcd_cursor_pos(0, 0);
       read_calibration_float(0x0fc0, a, &Kref);

       printf(ser_char, "%3.4f", Kref);

       lcd_cursor_pos(0, 10);
       read_calibration_float(0x0fc4, a, &Kbg);

       printf(ser_char, "%3.4f", Kbg);

       lcd_cursor_pos(1, 0);
       read_calibration_float(0x0fc8, a, &Vtherm);

       printf(ser_char, "%3.4f", Vtherm);

       lcd_cursor_pos(1, 10);
       read_calibration_float(0x0fcc, a, &Ktc);

       printf(ser_char, "%3.4f", Ktc);

	    lcd_cursor_pos(2, 0);
	    Fosc = read_calibration_byte(0x0fd0);
	    printf(ser_char, "%u", Fosc);

	    lcd_cursor_pos(2, 10);
	    Twdt = read_calibration_byte(0x0fd2);
       printf(ser_char, "%u", Twdt);

       delay_ms(5000);
    }
}

void read_calibration_float(unsigned long adr, byte *array, float *p_float)
{
#ifdef CERDIP1
   const byte cal[16] = {0x7c, 0x00, 0x94, 0x5a, 0x7f, 0x18, 0x14, 0xa8,
                         0x7f, 0x0d, 0xf7, 0x55, 0x76, 0x68, 0xa6, 0xe4};

#endif

#ifdef CERDIP2
   const byte cal[16] = {0x7c, 0x01, 0xd4, 0x76, 0x7f, 0x1a, 0xd7, 0xae,
                         0x7f, 0x04, 0xf0, 0x3d, 0x76, 0x69, 0x0e, 0x5b};
#endif

#ifdef CERDIP3
   const byte cal[16] = {0x7c, 0x02, 0x17, 0xa5, 0x7f, 0x17, 0x52, 0xf6,
                         0x7f, 0x08, 0xc8, 0x42, 0x76, 0x77, 0xe6, 0x50};
#endif

   byte n, *p;
#ifndef PRODUCTION

   for (n=0; n<4; n++)
   {
      array[n] = cal[adr - 0xfc0 + n];
   }
#else // Production

   for (n=0; n<4; n++)
   {
      array[n] = read_calibration(adr- 0xfc0 + n);	// fetch each byte in turn into a byte array
   }
#endif
   *p_float =  *((float *) array);	// copy the array to a float
}

byte read_calibration_byte(unsigned long adr)
{
#ifdef CERDIP1
   const byte cal[2] = {0xa3, 0x11};
#endif

#ifdef CERDIP2
   const byte cal[2] = {0x9d, 0x11};
#endif

#ifdef CERDIP3
   const byte cal[2] = {0xa7, 0x11};
#endif

#ifndef PRODUCTION
   if (adr == 0x0fd0)
   {
      return(cal[0]);
   }
   else
   {
      return(cal[1]);
   }
#else
	return(read_calibration(adr-0x0fc0));
#endif
}

#include <delay.c>
#include <ser_14.c>



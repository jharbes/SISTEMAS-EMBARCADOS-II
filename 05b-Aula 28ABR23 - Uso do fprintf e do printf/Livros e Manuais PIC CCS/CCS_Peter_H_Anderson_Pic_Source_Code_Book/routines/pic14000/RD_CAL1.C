// Rd_Cal1.c - PIC14000
//
// Reads calibration values from locations beginning at 0xfc0 and displays to
// the terminal.
//
// Test Program using PIC16F877
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
       for (n=0; n<4; n++)
       {
	 	  ser_hex_byte(a[n]);
       }

       lcd_cursor_pos(0, 10);
       read_calibration_float(0x0fc4, a, &Kbg);
	   for (n=0; n<4; n++)
	   {
		   ser_hex_byte(a[n]);
	   }

       lcd_cursor_pos(1, 0);
       read_calibration_float(0x0fc8, a, &Vtherm);
	   for (n=0; n<4; n++)
	   {
		   ser_hex_byte(a[n]);
	   }

       lcd_cursor_pos(1, 10);
       read_calibration_float(0x0fcc, a, &Ktc);
	   for (n=0; n<4; n++)
	   {
	 	   ser_hex_byte(a[n]);
	   }

	   lcd_cursor_pos(2, 0);
	   Fosc = read_calibration_byte(0x0fd0);
	   ser_hex_byte(Fosc);

	   lcd_cursor_pos(2, 10);
	   Twdt = read_calibration_byte(0x0fd2);
       ser_hex_byte(Twdt);

      delay_ms(5000);
    }
}

void read_calibration_float(unsigned long adr, byte *array, float *p_float)
{
   byte n;

   for (n=0; n<4; n++)
   {
      array[n] = read_calibration(adr- 0xfc0 + n);	// fetch each byte in turn into a byte array
   }
   *p_float = (float) *array;	// copy the array to a float
}

byte read_calibration_byte(unsigned long adr)
{
	return(read_calibration(adr-0x0fc0));
}

#include <delay.c>
#include <ser_14.c>

#rom 0x07bf = {0x0782}

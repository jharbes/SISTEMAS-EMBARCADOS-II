// 1820_1.C (PIC16F628)
//
// Ilustrates implementation of Dallas 1-wire interface.
//
// Continually loops, reading nine bytes of data from DS1820 thermometer
// devices on PORTB7 - PORTB4 and displays the results for each sensor on
// serial LCD.  Note that the assignment is specified in _1_WIRE.H.
//
// PIC16F628							DS1820
//
// PORTB7 (term 13) ---------------------- DQ
// PORTB6 (term 12) ---------------------- DQ
// PORTB5 (term 11) ---------------------- DQ
// PORTB4 (term 10) ---------------------- DQ
//
// Note that a 4.7K pullup resistor to +5 VDC is on each DQ lead.
//
// copyright, Peter H. Anderson, Baltimore, MD, Apr, '01

#case

#device PIC16F628 *=16

#include <defs_628.h>
#include <ser_628.h>
#include <delay.h>
#include <_1_wire.h>

#define FALSE 0
#define TRUE !0

#define MAX_SENSORS 4

void set_next_line(byte *p);

void main(void)
{
   byte buff[9], sensor, n, line;

   ser_init();
   ser_lcd_init();

   while(1)
   {
      ser_lcd_clr_all();

      for (sensor=0; sensor<MAX_SENSORS; sensor++)
      {
         line = 0;
         if(!_1w_init(sensor)) // if a DS1820 is not detected
         {
		     ser_lcd_clr_line(line);
             ser_hex_byte(sensor);
             set_next_line(&line);

	   	     ser_lcd_clr_line(line);
		     printf(ser_char, "Not Detected");
		     set_next_line(&line);
		     delay_ms(500);
	 }

	 else	// otherwise, perform a temperature meas
	 {
            _1w_out_byte(sensor, 0xcc);  // skip ROM

            _1w_out_byte(sensor, 0x44);  // perform temperature conversion
            _1w_strong_pull_up(sensor);

            _1w_init(sensor);
            _1w_out_byte(sensor, 0xcc);  // skip ROM

            _1w_out_byte(sensor, 0xbe);

            for (n=0; n<9; n++)			// fetch the nine bytes
            {
               buff[n]=_1w_in_byte(sensor);
            }

            ser_lcd_clr_line(line);
            ser_hex_byte(sensor);		// display the sensor number
            set_next_line(&line);

	        ser_lcd_clr_line(line);
            for (n=0; n<4; n++)			// and the results
            {
               ser_hex_byte(buff[n]);
               ser_char(' ');
            }

            set_next_line(&line);
            ser_lcd_clr_line(line);

            for (n=4; n<9; n++)
            {
               ser_hex_byte(buff[n]);
               ser_char(' ');
            }


            delay_ms(500);
         }	// end of else
     }  // of for
     delay_ms(1000);
  } // of while
}


void set_next_line(byte *p)
{
	++(*p);
	if (*p == 4)
	{
		*p = 0;
	}
}

#include <ser_628.c>
#include <delay.c>
#include <_1_wire.c>

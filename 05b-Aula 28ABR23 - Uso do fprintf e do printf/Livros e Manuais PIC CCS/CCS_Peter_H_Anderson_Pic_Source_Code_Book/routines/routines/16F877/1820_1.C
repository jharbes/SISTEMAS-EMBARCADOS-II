// 1820_1.C
//
// Ilustrates implementation of Dallas 1-wire interface.
//
// Continually loops, reading nine bytes of data from DS1820 thermometer
// devices on PORTD0 - PORTD3 and displays the results for each sensor on
// the LCD.
//
// PIC16F877							DS1820
//
// PORTD3 (term 22) ---------------------- DQ
// PORTD2 (term 21) ---------------------- DQ
// PORTD1 (term 20) ---------------------- DQ
// PORTD0 (term 19) ---------------------- DQ
//
// Note that a 4.7K pullup resistor to +5 VDC is on each DQ lead.
//
// copyright, Peter H. Anderson, Georgetown, SC,, Mar, '01

#case

#device PIC16F877 *=16 ICD=TRUE

#include <defs_877.h>
#include <lcd_out.h>
#include <_1_wire.h>

#define FALSE 0
#define TRUE !0

#define MAX_SENSORS 4

void set_next_line(byte *p);

void main(void)
{
   byte buff[9], sensor, n, line = 0;

   pspmode = 0;	// configure parallel slave port as general purpose port

   while(1)
   {
      lcd_init();

      for (sensor=0; sensor<MAX_SENSORS; sensor++)
      {
         line = 0;
         if(!_1w_init(sensor)) // if a DS1820 is not detected
         {
		     lcd_clr_line(line);
             lcd_hex_byte(sensor);
             set_next_line(&line);

	   	     lcd_clr_line(line);
		     printf(lcd_char, "Not Detected");
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

            lcd_clr_line(line);
            lcd_hex_byte(sensor);		// display the sensor number
            set_next_line(&line);

	        lcd_clr_line(line);
            for (n=0; n<4; n++)			// and the results
            {
               lcd_hex_byte(buff[n]);
               lcd_char(' ');
            }

            set_next_line(&line);
            lcd_clr_line(line);

            for (n=4; n<9; n++)
            {
               lcd_hex_byte(buff[n]);
               lcd_char(' ');
            }


            delay_ms(500);
         }	// end of else
     }  // of for
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

#include <lcd_out.c>
#include <_1_wire.c>

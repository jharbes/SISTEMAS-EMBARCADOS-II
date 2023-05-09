// SER_ADC.C (PIC16F628)
//
// Interfaces with BasicX Serial LCD+ to perform A/D conversion.
//
// PIC sends the control code 0x16 followed by the channel (1-8)
// and then receives the two byte result.  Note that the format of
// the returned data is low byte followed by high byte.
//
// Displays the result in 4-nibble hexadecimal.
//
// PIC16F628			Serial LCD+
//
//  RB2/TX (term 8) ----------> (term 2)
//  RB1/RX (term 7) <---------- (term 1)
//
// Uses files ser_628 and delay.c.
//
// copyright, Peter H. Anderson, Baltimore, MD, April, '01

#case

#device PIC16F628 *=16

#include <defs_628.h>
#include <ser_628.h>
#include <delay.h>

long ser_lcd_adc_meas(byte channel);

void main(void)
{
	long ad_val;

	ser_init();
	ser_lcd_init();
	ser_lcd_set_beep_freq(100);

	while(1)
	{
	    ser_lcd_clr_line(0);			// clear line 0
	    ad_val = ser_lcd_adc_meas(1);	// perform A/D measurement
	    ser_lcd_beep();
	    ser_hex_byte(ad_val >> 8);		// display high byte in hex
	    ser_hex_byte(ad_val & 0xff);	// followed by the low byte
	    delay_ms(500);
	}
}

long ser_lcd_adc_meas(byte channel)
{
	byte ch, ad_lo, ad_hi, n;
	long ad_val;

	ch = RCREG;							// be sure UART is clear
	ch = RCREG;

	ser_char(0x16);						// send command
	ser_char(channel);

	if ((ad_lo = ser_get_ch(100)) == 0xff)
	{
		return(0x7fff);					// if no response
	}
	if ((ad_hi = ser_get_ch(100)) == 0xff)
	{
		return(0x7fff);					// if no second byte received
	}

	/* else */
	ad_val = ad_hi;
	ad_val = (ad_val << 8) | ad_lo;
	return(ad_val);
}

#include <ser_628.c>
#include <delay.c>

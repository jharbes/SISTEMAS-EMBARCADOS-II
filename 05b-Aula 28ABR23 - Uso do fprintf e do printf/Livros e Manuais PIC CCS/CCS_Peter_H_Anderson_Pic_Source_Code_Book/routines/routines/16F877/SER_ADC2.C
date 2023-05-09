#case

#device PIC16F877 *=16 ICD=TRUE

#include <a:\16f877\defs_877.h>
#include <a:\16f877\ser_87x.h>
#include <a:\16f877\delay.h>

long ser_lcd_adc_meas(byte channel);

void main(void)
{
	long ad_val;

	ser_init();
	ser_lcd_init();
	ser_lcd_set_beep_freq(100);

	while(1)
	{
	    
       ser_lcd_clr_line(1);
	    ad_val = ser_lcd_adc_meas(1);
	    ser_lcd_beep();
	    ser_hex_byte(ad_val >> 8);
	    ser_hex_byte(ad_val & 0xff);
	    delay_ms(500);
	}
}

long ser_lcd_adc_meas(byte channel)
{
	byte ch, ad_lo, ad_hi, n;
	long ad_val;

	ch = RCREG;
	ch = RCREG;

	ser_char(0x16);
	ser_char(channel);

	if ((ad_lo = ser_get_ch(100)) == 0xff)
	{
		return(0x7fff);
	}
	if ((ad_hi = ser_get_ch(100)) == 0xff)
	{
		return(0x7fff);
	}

	/* else */
	ad_val = ad_hi;
	ad_val = (ad_val << 8) | ad_lo;
	return(ad_val);
}

#include <a:\16f877\ser_87x.c>
#include <a:\16f877\delay.c>

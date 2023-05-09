#include <main.h>

//#define use_portd_kbd TRUE // default
#include <KBD.C>

#define LCD_DATA_PORT getenv("SFR:PORTB")   
#include <lcd.c>


void main()
{
	char k;

	setup_adc_ports(NO_ANALOGS, VSS_VDD);
	kbd_init();

   lcd_init();

	lcd_putc("\fDigite uma tecla\n");

	while(TRUE)
	{

		//Example using external LCD
		k = kbd_getc();
		if(k!=0)
			if(k=='*')
				lcd_putc('\f');
			else
				lcd_putc(k);

	}

}

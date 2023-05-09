#include <main.h>

void main()
{
	setup_adc_ports(NO_ANALOGS, VSS_VDD);

	while(TRUE)
	{

		//Example blinking LED program
		output_low(LED);
		delay_ms(DELAY);
		output_high(LED);
		delay_ms(DELAY);

		//TODO: User Code
	}

}

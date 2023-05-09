#include <18F4520.h>
#use delay(internal=8Mhz)

void main(void)
{

	while(1)
	{
	output_high(PIN_B0);
	delay_ms(1000);
	output_low(PIN_B0);
	delay_ms(1000);
	}

}

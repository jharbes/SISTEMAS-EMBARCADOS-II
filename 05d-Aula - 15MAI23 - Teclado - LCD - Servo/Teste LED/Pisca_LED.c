#include <18F4520.h>

#use delay(internal=1000000)


void main(void){

	while(TRUE){
 	
	output_high(PIN_B0);
	delay_ms(200);
	output_low(PIN_B0);
	delay_ms(200); 	
 	
	}

}

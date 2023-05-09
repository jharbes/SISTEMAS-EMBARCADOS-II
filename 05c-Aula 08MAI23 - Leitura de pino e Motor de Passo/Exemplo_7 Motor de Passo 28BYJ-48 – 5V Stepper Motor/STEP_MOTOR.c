#include <STEP_MOTOR.h>

void gira_direita( int16 pulso, int16 giros);	// considerou-se xx graus cada step
void gira_esquerda( int16 pulso, int16 giros);// cada giro equivale a uma sequ�ncia
																							// de 4 pulsos
int8 passo;


void main()
{
//passo =	Step_1_8_graus; // motor de passo de 1.8graus
passo =	Step_5_graus;     // motor de passo de 5.625 graus
//passo =	Step_10_graus;  // motor de passo de 10graus

output_low(PIN_D0);
output_low(PIN_D1);
output_low(PIN_D2);
output_low(PIN_D3);
delay_ms(1000);

	while(TRUE)
	{
	gira_direita( largura_pulso, 1);
	delay_ms(1000);
	gira_esquerda(largura_pulso, 1);
	delay_ms(1000); 	
	}

}

void gira_direita( int16 pulso, int16 giros){

int16 i;

for(i=0; i<giros*passo; i++){

	output_high(PIN_D0);
	delay_ms(pulso);
	output_low(PIN_D0);
 	
	output_high(PIN_D1);
	delay_ms(pulso);
	output_low(PIN_D1);
 	
	output_high(PIN_D2);
	delay_ms(pulso);
	output_low(PIN_D2);
 	
	output_high(PIN_D3);
	delay_ms(pulso);
	output_low(PIN_D3);
	}
}

void gira_esquerda( int16 pulso, int16 giros){

int16 i;

for(i=0; i<giros*passo; i++){

	output_high(PIN_D3);
	delay_ms(pulso);
	output_low(PIN_D3);
 	
	output_high(PIN_D2);
	delay_ms(pulso);
	output_low(PIN_D2);
 	
	output_high(PIN_D1);
	delay_ms(pulso);
	output_low(PIN_D1);
 	
	output_high(PIN_D0);
	delay_ms(pulso);
	output_low(PIN_D0);
	}
}

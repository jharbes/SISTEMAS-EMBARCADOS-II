#include <18F4520.h>

#FUSES NOMCLR         				//No MASTER CLEAR

#use delay(internal=1000000)

#define Step_1_8_graus	50	// 200/4 = n�mero de repeti��es para uma rota��o 360 graus
														// motor de passo de 1.8graus
#define Step_10_graus 	9		// 36/4
														// motor de passo de 10graus
													 	
#define Step_5_graus	16	// 64/4 = n�mero de repeti��es para uma rota��o 360 graus
														// motor 28BYJ-48 - 1 volta = 64 pulsos, ou seja, 5.625 graus por pulso
													 	
#define largura_pulso   50 // determina a velocidade de rota��o

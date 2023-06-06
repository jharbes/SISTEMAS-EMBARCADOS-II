#include <18F46K22.h>

#FUSES INTRC_IO		//Internal RC Osc, no CLKOUT
#FUSES MCLR				// pino de Master Clear ativo									 	

#use delay(int=8000000)


// acesso aos pinos:
#define LCD_ENABLE_PIN  PIN_E1
#define LCD_RS_PIN      PIN_E0
#define LCD_RW_PIN      PIN_E2
#define LCD_DATA4       PIN_D4
#define LCD_DATA5       PIN_D5
#define LCD_DATA6       PIN_D6
#define LCD_DATA7       PIN_D7 


#define SERVO_ENTRADA   PIN_B7
#define SERVO_SAIDA     PIN_B6

#define SET_VAGAS 			PIN_B3
#define SENSOR_ENTRADA  PIN_B4
#define SENSOR_SAIDA    PIN_B5

#rom 0xf00000={8,10,20,30,40} //insere dados na EEPROM interna durante a grava��o do PIC
// podem ser senhas ou outros vaolres que devem ser salvos durante o tempo de execu��o
// e disponibilizados posteriormente
// nesse exemplo, somente o primeiro valor ser� utilizado
// o endere�o de in�cio da EEPROM interna varia com a fam�lia do PIC
// veja no Tools/Device Editor o valor de "Data EE Start" em "Memory" para o PIC escolhido

// Prot�tipos das fun��es
void abre_entrada(void);
void fecha_entrada(void);
void abre_saida(void);
void fecha_saida(void);

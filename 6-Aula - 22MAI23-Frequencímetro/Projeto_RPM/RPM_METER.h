#include <18F4520.h>
#device adc=10


#FUSES INTRC_IO                 //Internal RC Osc, no CLKOUT
#FUSES MCLR										// reset ativo

#use delay(int=4000000)

// acesso aos pinos:
#define LCD_ENABLE_PIN  PIN_E1
#define LCD_RS_PIN      PIN_E0
#define LCD_RW_PIN      PIN_E2
#define LCD_DATA4       PIN_D4
#define LCD_DATA5       PIN_D5
#define LCD_DATA6       PIN_D6
#define LCD_DATA7       PIN_D7 


#USE TIMER(TIMER=1,TICK=1us,BITS=32,ISR)

//#rom getenv("EEPROM_ADDRESS")={0,0,0,0};// escreve nos 4 primeiros endereços da EEPROM
// o PIC18F4520 tem 256 bytes de EEPROM
// equivale à linha de baixo
//#rom 0xf00000={0,0,0,0,0} 





#include <18F4520.h>
//#device adc=10


#FUSES INTRC_IO                 //Internal RC Osc, no CLKOUT
#FUSES MCLR										  // reset ativo

#use delay(int=4000000)

// acesso aos pinos:
#define LCD_ENABLE_PIN  PIN_E1
#define LCD_RS_PIN      PIN_E0
#define LCD_RW_PIN      PIN_E2
#define LCD_DATA4       PIN_D4
#define LCD_DATA5       PIN_D5
#define LCD_DATA6       PIN_D6
#define LCD_DATA7       PIN_D7 





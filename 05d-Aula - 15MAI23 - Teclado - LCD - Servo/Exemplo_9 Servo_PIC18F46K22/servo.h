#include <18F46K22.h>
#device ADC=10

#use delay(internal=8MHz)
#use rs232(baud=9600,parity=N,xmit=PIN_C6,rcv=PIN_C7,bits=8,stream=PORT1)

// acesso aos pinos:
#define LCD_ENABLE_PIN  PIN_E1
#define LCD_RS_PIN      PIN_E0
#define LCD_RW_PIN      PIN_E2
#define LCD_DATA4       PIN_D4
#define LCD_DATA5       PIN_D5
#define LCD_DATA6       PIN_D6
#define LCD_DATA7       PIN_D7 

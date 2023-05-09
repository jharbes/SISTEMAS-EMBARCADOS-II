#include <18F4520.h>
#device ADC=10 // conversor A/D de 10 bits

#use delay(internal=8MHz,restart_wdt)
#use rs232(baud=9600,parity=N,xmit=PIN_C6,rcv=PIN_C7,bits=8,stream=PORT1)
#use rs232(baud=9600,parity=N,xmit=PIN_C4,rcv=PIN_C5,bits=8,stream=PORT2,FORCE_SW)
#use rs232(baud=9600,parity=N,xmit=PIN_C2,rcv=PIN_C3,bits=8,stream=PORT3,FORCE_SW)

// defini��es dos pinos do LCD
#define LCD_ENABLE_PIN PIN_D0
#define LCD_RS_PIN PIN_D1
#define LCD_RW_PIN PIN_D2
#define LCD_DATA4 PIN_D4
#define LCD_DATA5 PIN_D5
#define LCD_DATA6 PIN_D6
#define LCD_DATA7 PIN_D7

float Vin_0;// valor bruto do AD - varia de 0 a 1023
float Vin_1;
float Vin_2;

float V_0;// valor convertido para a faixa de 0  5.00V
float V_1;
float V_2;



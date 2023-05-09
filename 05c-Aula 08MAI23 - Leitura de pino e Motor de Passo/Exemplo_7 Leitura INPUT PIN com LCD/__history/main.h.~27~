#include <18F4520.h>
#use delay(internal=8MHz)

// definições dos pinos do LCD (podem ser utilizados vários tipos de LCD de 
// duas linhas: 16x2; 20x2; 40x2; e de uma linha: 16x1; 20x1; 40x1)
// para displays com 4 linhas, veja <lcd420.c>

// Configuração utilizando pinos de uma ou mais portas
#define LCD_ENABLE_PIN	PIN_D0 // LCD_ENABLE_PIN, LCD_RS_PIN e LCD_RW_PIN
#define LCD_RS_PIN			PIN_D1 // podem estar em quaisquer portas
#define LCD_RW_PIN			PIN_D2
#define LCD_DATA4 			PIN_D4 // LCD_DATA4, LCD_DATA5, LCD_DATA6 e LCD_DATA7 
#define LCD_DATA5 			PIN_D5 // devem estar na mesma porta. Aqui foi esvolhida
#define LCD_DATA6 			PIN_D6 // a porta D.
#define LCD_DATA7 			PIN_D7

// Configuração utilizando pinos de uma única porta nessa ordem
// (pode ser alterada em lcd.c linha 80, alterando a struct LCD_PIN_MAP)

//#define LCD_DATA_PORT getenv("SFR:PORTC")  // LCD em PORTC 

// LCD_ENABLE_PIN  PIN_C0
// LCD_RS_PIN      PIN_C1  
// LCD_RW_PIN      PIN_C2 
// Não conectado	 PIN_C3
// LCD_DATA4       PIN_C4
// LCD_DATA5       PIN_C5
// LCD_DATA6       PIN_C6
// LCD_DATA7       PIN_C7

//#define LCD_DATA_PORT getenv("SFR:PORTD")  // LCD em PORTD 

// LCD_ENABLE_PIN  PIN_D0
// LCD_RS_PIN      PIN_D1  
// LCD_RW_PIN      PIN_D2
// Não conectado	 PIN_D3
// LCD_DATA4       PIN_D4
// LCD_DATA5       PIN_D5
// LCD_DATA6       PIN_D6
// LCD_DATA7       PIN_D7


#define LCD_EXTENDED_NEWLINE
// LCD_EXTENDED_NEWLINE is defined
// the \n character will erase all remanining characters on the current line
// and move the cursor to the beginning of the next line;                                                 
// the \r character will move the cursor to the start of the current line.


/////////////////////////////////////////////////////////////////////////
////                      PROJETO   FREQUENC�METRO                   ////
/////////////////////////////////////////////////////////////////////////

#include "FREQ.h"
#include <lcd.c>

int32 freq=0;
int32 overflow_timer0=0;

	
#INT_TIMER0 // 
void  TIMER0_isr(void) 
{
overflow_timer0++;
}


void main() {

   setup_oscillator(OSC_4MHZ);  
  
  // Timer0 configurado como 16 bits (se fosse T0_8_BIT, seria 8 bits), veja abaixo
  // SETUP_TIMER_0(T0_EXT_L_TO_H | T0_DIV_1 | T0_8_BIT);
  // o que permite Timer0 conte at� 65535 pulsos antes do overflow
  // Timer0 usar� o Clock Externo, via pino 6 do PIC (T0CKI- Timer0 Clock Input)
  // que recebe os pulsos vindos do gerador 
   SETUP_TIMER_0(T0_EXT_L_TO_H | T0_DIV_1 );// clock EXT, conta na subida do clock, sem pre-divis�o
   
   enable_interrupts(INT_TIMER0);// habilita as interrup��es de Timer0, as quais incrementam overflow_timer0
  															// a cada 65536 pulsos do gerador
   enable_interrupts(GLOBAL);


   lcd_init();// inicializa o LCD
   lcd_putc(" Frequencimetro \n");// escreve no LCD a mensagem

   while (TRUE) {
    		  	 	
  		set_timer0(0);// inicializa o Timer0 com 0
  		overflow_timer0=0;// inicializa o overflow_timer0 com 0
  		delay_ms(1000);// espera 1s enquanto o Timer0 conta os pulsos vindos do gerador de sinais
  		freq=(get_timer0() + 65536*overflow_timer0);// calcula quantos pulsos chegaram ao clock de Timer0
																								// incluindo o n�mero de overflows multiplicado por 65536
																								// Por exemplo: se a frequ�ncia for 100kHz, em 1s de contagem, 
																								//  ser�o 100000 pulsos, o que ir� provocar um overflow
																								// em Timer0, quando a contagem volta a zero, e continua contando.
																								// Ap�s 1s de contagem, teremos 1 overflow e uma contagem em Timer0 de: 
																								// 100000 - 65536 = 34464. Ao se multiplicar o n�mero de overflows
																								// por 65536 e se somar o conte�do de Timer0, se contabilizam todos
																								// os pulsos ocorridos em 1s, o que � a express�o da frequ�ncia em Hz
																								 
      lcd_gotoxy(1,2);
      printf(LCD_PUTC, "Freq:%LUHz     ",freq); // exibe o valor da frequ�ncia
   }
}


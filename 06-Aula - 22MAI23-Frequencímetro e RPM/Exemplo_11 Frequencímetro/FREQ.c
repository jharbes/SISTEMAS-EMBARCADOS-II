/////////////////////////////////////////////////////////////////////////
////                      PROJETO   FREQUENCÍMETRO                   ////
/////////////////////////////////////////////////////////////////////////

#include "FREQ.h"
#include <lcd.c>

int32 freq=0;
int32 overflow_timer0=0;
int1 T1_contando=0;

	
#INT_TIMER0 // 
void  TIMER0_isr(void) 
{
overflow_timer0++;
}

#INT_TIMER1 // 
void  TIMER1_isr(void) 
{
T1_contando=0;
}



void main() {

   setup_oscillator(OSC_1MHZ);  
  
  // Timer0 configurado como 16 bits (se fosse T0_8_BIT, seria 8 bits), veja abaixo
  // SETUP_TIMER_0(T0_EXT_L_TO_H | T0_DIV_1 | T0_8_BIT);
  // o que permite Timer0 conte até 65535 pulsos antes do overflow
  // Timer0 usará o Clock Externo, via pino 6 do PIC (T0CKI- Timer0 Clock Input)
  // que recebe os pulsos vindos do gerador 
   SETUP_TIMER_0(T0_EXT_L_TO_H | T0_DIV_1 );// clock EXT, conta na subida do clock, sem pre-divisão
   

   setup_timer_1 ( T1_INTERNAL | T1_DIV_BY_8 );// clock interno dividido por 8, fclock=1MHz/4*8
  																						//  Período=32us, overflow= 65536*32= 2.097152s
  																						// 1s=1000000us; que equivale a 1000000/32= 31250 pulso de 32us;
  																						// para Timer1 ter overflow precisamos
  																						// de ter 65536 pulsos; fazemos o preset do Timer
  																						// com 65536-31250=34286 e esperamos o overflow em 1s
   
   enable_interrupts(INT_TIMER0);// habilita as interrupções de Timer0, as quais incrementam overflow_timer0
  															// a cada 65536 pulsos do gerador
   enable_interrupts(GLOBAL);


   lcd_init();// inicializa o LCD
   lcd_putc(" Frequencimetro \n");// escreve no LCD a mensagem

   while (TRUE) {
    		  	 	
  	set_timer0(0);// inicializa o Timer0 com 0
  	overflow_timer0=0;// inicializa o overflow_timer0 com 0
  	 	
		T1_contando = 1;
  	//set_timer1(34286);// inicializa o Timer1 com 34286 para termos overflow em 65536 pulsos (1s)
  	set_timer1(34286+13);// inicializa o Timer1 com 34286 para termos overflow em 65536 pulsos (1s) 13 pulso a menos para compensar outros códigos
  	enable_interrupts(INT_TIMER1);// habilita as interrupções de Timer1
 	 while(T1_contando); // espera a interrupção do Timer1 fazer T1_contando=0
 	 disable_interrupts(INT_TIMER1);
  	  
//		delay_ms(1000);// espera 1s enquanto o Timer0 conta os pulsos vindos do gerador de sinais
  	freq=(get_timer0() + 65536*overflow_timer0);// calcula quantos pulsos chegaram ao clock de Timer0
																								// incluindo o número de overflows multiplicado por 65536
																								// Por exemplo: se a frequência for 100kHz, em 1s de contagem, 
																								//  serão 100000 pulsos, o que irá provocar um overflow
																								// em Timer0, quando a contagem volta a zero, e continua contando.
																								// Após 1s de contagem, teremos 1 overflow e uma contagem em Timer0 de: 
																								// 100000 - 65536 = 34464. Ao se multiplicar o número de overflows
																								// por 65536 e se somar o conteúdo de Timer0, se contabilizam todos
																								// os pulsos ocorridos em 1s, o que é a expressão da frequência em Hz
																								 
    lcd_gotoxy(1,2);
    printf(LCD_PUTC, "Freq:%LUHz     ",freq); // exibe o valor da frequência
   }
}


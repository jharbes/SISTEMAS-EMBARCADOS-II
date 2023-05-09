/////////////////////////////////////////////////////////////////////////
////                           PROJETO   RPM                         ////
/////////////////////////////////////////////////////////////////////////

#include "RPM_METER.h"
#include <lcd.c>
int32 metros=0;// salva quantos metros o ve�culo se deslocou (odometria)
float kilometros=0;// recebeo a odometria convertida em quil�metros
unsigned int32 periodo_RPM=0;// recebe o per�odo do sinal do sensor de RPM em us
unsigned int32 periodo_roda=0;// recebe o per�odo do sinal do sensor de roda em us
float x_RPM=0;// valor da velocidade de rota��o do motor calculado a partir do pr�odo do sensor de RPM em us
float velocidade=0;// valor da velocidade do ve�culo calculado a partir do pr�odo do sensor de roda em us
unsigned int32 current_tick_RPM=0;// usado para o sensor de RPM, valor atual
unsigned int32 previous_tick_RPM=0;// usado para o sensor de RPM, valor passado

unsigned int32 current_tick_RPM_WDT;// usado para o sensor de RPM para detectar motor parado
unsigned int32 previous_tick_RPM_WDT;// usado para o sensor de RPM para detectar motor parado

unsigned int32 current_tick_roda=0;// usado para o sensor da roda
unsigned int32 previous_tick_roda=0;// usado para o sensor de roda, valor passado

// prot�tipo da fun��o que calcula quantos ticks (us) foram contados entre uma interrup��o
// e outra, usado para as interrup��es externas #INT_EXT e #INT_EXT2 (veja abaixo)
unsigned int32 tick_difference(unsigned int32 current, unsigned int32 previous);


// calcula quantos us transcorreram entre interrup��es do sensor de RPM
// utiliza o contador de ticks implementado com o Timer1 (veja em RPM_METER.h): #USE TIMER(TIMER=1,TICK=1us,BITS=32,ISR)
#INT_EXT // RPM
void  EXT_isr(void) 
{
current_tick_RPM = get_ticks();// l� o valor atual do contador de ticks
periodo_RPM = tick_difference(current_tick_RPM, previous_tick_RPM);// calculo do per�odo do sinal do sensor de RPM
//periodo_RPM = current_tick_RPM - previous_tick_RPM;// calculo do per�odo do sinal do sensor de RPM
// salva a contagem de ticks atual para se utilizar na pr�xima 
// interrup��o como valor passado	 	
previous_tick_RPM = current_tick_RPM;

// salva a contagem de ticks a cada interrup��o para se verificar no final do "while(TRUE)"
// se	o sensor de RPM est� a mais de 1s sem enviar pulos, o que informar� que o motor est�
// parado e o valor de RPM que ser� mostrado no LCD deve ser 0RPM
previous_tick_RPM_WDT= get_ticks();
}																												  


// 1) incrementa a cada pulso proveniente do sensor da roda (1m) para o c�lculo da odometria
// 2) calcula quantos us transcorreram entre interrup��es do sensor da roda
//    utiliza o contador de ticks implementado com o Timer1 
//    (veja em RPM_METER.h): #USE TIMER(TIMER=1,TICK=1us,BITS=32,ISR)
#INT_EXT2 // Od�metro e velocidade
void  EXT2_isr(void) 
{
metros=metros+1;// c�lculo da odometria

current_tick_roda = get_ticks();// l� o valor atual do contador de ticks
periodo_roda = tick_difference(current_tick_roda, previous_tick_roda);// c�lculo do per�odo do sensor de roda

// salva a contagem de ticks atual para se utilizar na pr�xima
// interrup��o como valor passado
previous_tick_roda = current_tick_roda;                               
}																															 	

void main() {
   float valor;// bin�rio de 10 bits do A/D
   float tensao;// bin�rio do A/D convertido para volts (0.0 a 15.0V) 

   setup_adc(ADC_CLOCK_INTERNAL);// Conversor A/D usar� o oscilador interno   
   setup_adc_ports(AN0);// usa AN0 para ler a tens�o da bateria dividida por 3
   set_adc_channel(0);// seleciona o AN0 para se lido
   
   enable_interrupts(INT_EXT);// contar� o tempo entre duas interrup��es do sensor de RPM
   enable_interrupts(INT_EXT2);// contar� o tempo entre duas interrup��es do sensor de Roda
  														 // e quantos metros foram percorridos
   enable_interrupts(GLOBAL);


   lcd_init();
   lcd_putc("\fVelocidade   RPM\n");
   lcd_putc("Odometro Bateria");   
   delay_ms(500);
 	
	// recupera valor de metros percorridos salvo na EEPROM 
	// experimente comentar essa linha e recompilar
	// se tirar essa linha, ao se resetar o PIC o od�metro zera
   metros= make32(read_eeprom(3),read_eeprom(2),read_eeprom(1),read_eeprom(0));

   while(TRUE) {
    
      valor= read_adc();// l� o canal o do ADC (AN0)          
      delay_us(100);// aguarda a conclus�o da convers�o AD 
      tensao= (valor*15) /1024;// converte o valor bin�rio para volts
      
      x_RPM= 60000000/periodo_RPM;// 60*1000000/ T (us)
      
      // a cada itera��o do "while(TRUE)", calcula-se o tempo em us decorrido entre a �ltima
      // interrup��o do sensor de RPM e o instante atual. Se esse tempo for maior que 1s (1000000us),
      //  zerar o valor de "x_RPM" (motor parado)      
      current_tick_RPM_WDT=get_ticks();
      if(tick_difference(current_tick_RPM_WDT, previous_tick_RPM_WDT)> 1000000){
      x_RPM=0;
      }  
  
     
      velocidade= 3600000/periodo_roda;// velocidade em km/h
      
      if(input(PIN_C0)==0) metros=0;// zera o od�metro;
   
  		// salva o n�mero de metros na eeprom interna a cada itera��o do while(TRUE)
  		write_eeprom(0,metros);// pega somente os 8 bits menos gignificativos de int32 metros (D0 a D7)
  		write_eeprom(1,metros>>8);// pega 8 bits de int32 metros (D8 a D15)
  		write_eeprom(2,metros>>16);// pega 8 bits de int32 metros (D16 a D23)
  		write_eeprom(3,metros>>24);// pega 8 bits de int32 metros (D24 a D31)
  	 	
  		kilometros= (float)metros/1000;// "metros" foi promovido a "float" para evitar-se o truncamento 
  																	// da parte decimal do resultado, que � salvo em "kilometros"
  	 	
      lcd_gotoxy(1,1);
      printf(LCD_PUTC, "%3.0fkm/h %5.0fRPM\n",velocidade,x_RPM); //and print RPM
      printf(LCD_PUTC, "%8.3fkm %3.1fV",kilometros,tensao);      //and print metros percorridos
     
      delay_ms(10);
   }
}

unsigned int32 tick_difference(unsigned int32 current, unsigned int32 previous) {

   return(current - previous);
}

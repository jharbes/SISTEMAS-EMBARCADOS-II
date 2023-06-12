#include <main.h>
#include <lcd.c>

void main(){
   setup_adc_ports(AN0_TO_AN2, VSS_VDD); // Configura AN0, AN1 e AN2 como entradas analógicas
   setup_adc(ADC_CLOCK_INTERNAL | ADC_TAD_MUL_20);// Inicia o conersor A/D            

  set_adc_channel(0);                       // Seleciona o canal do conersor A/D para AN0
  lcd_init();
   lcd_putc("\fPartida...\n");
   delay_ms(1000);
// Teste dos LEDS
   output_high(LED_TEMP_BAIXA);
   delay_ms(200);
   output_low(LED_TEMP_BAIXA);    
   output_high(LED_TEMP_NORMAL);
   delay_ms(200);
   output_low(LED_TEMP_NORMAL);    
   output_high(LED_TEMP_ALTA);
   delay_ms(200);
   output_low(LED_TEMP_ALTA);
    
   output_high(LED_PRESSAO_BAIXA);
   delay_ms(200);
   output_low(LED_PRESSAO_BAIXA);   
   output_high(LED_PRESSAO_NORMAL);
   delay_ms(200);
   output_low(LED_PRESSAO_NORMAL);    
   output_high(LED_PRESSAO_ALTA);
   delay_ms(200);
   output_low(LED_PRESSAO_ALTA);    
    

   while(TRUE){
    set_adc_channel(0);                       // Seleciona o canal do conersor A/D para AN0
   Vin_0= read_adc();                        // Leia o valor do canal 0 em bináriode 10 bits
   set_adc_channel(1);                       // Seleciona o canal do conersor A/D para AN1
   Vin_1= read_adc();                        // Leia o valor do canal 1 em bináriode 10 bits   
   set_adc_channel(2);                       // Seleciona o canal do conersor A/D para AN2
   Vin_2= read_adc();                        // Leia o valor do canal 2 em binário de 10 bits
   
  // Calcula os valores de V_0, V_1 e V_2 em volts
  V_0=(Vin_0*5)/1024*100;
  V_1=(Vin_1*5)/1024*100;   
  V_2=(((Vin_2*5)/1024)-0.204)*53;
          
  
  fprintf(PORT1,"Mateus Rangel\n\r");
  
                                                         
  // Envia os valores de V_0, V_1 e V_2 em volts para a saída serial com 1 algarismo inteiros e duas casas decimais
  
  fprintf(PORT2,"Tc=%1.0gC  Ta=%1.0gC  Pc=%1.0g Kpa\r\n",V_0,V_1,V_2);// V_1 vai ocupar 6 espaços                                                                                           // com uma casa decimal e o ponto decimal conta                                                                                        
  printf(lcd_putc,"\fV0=%1.0gC Ta=%1.2gV \n",V_0,V_1);// escreve no LCD formatado    
  printf(lcd_putc,"Pc=%1.0g Kpa",V_2);    
  
  //Definindo as cores dos Leds de acordo com a temperatura
  if (V_0 < 110){
  output_high(LED_TEMP_BAIXA);
  fprintf(PORT3,"Temperatura Baixa\r"); 
  }
  else {
  output_low(LED_TEMP_BAIXA);
  }
  
  if (V_0 >= 110 && V_0 <= 130){
   output_high(LED_TEMP_NORMAL);
   fprintf(PORT3,"Temperatura Normal\r");
  }
  else {
  output_low(LED_TEMP_NORMAL); 
  }
  
  if (V_0 >= 130){
   output_high(LED_TEMP_ALTA);
   fprintf(PORT3,"Temperatura Alta\r");
  }
  else {
  output_low(LED_TEMP_ALTA); 
  }

   //Definindo as Cores dos Leds de acordo com a pressão
  
  if (V_2 < 150){
  output_high(LED_PRESSAO_BAIXA);
  fprintf(PORT3,"Pressao Baixa\r");
  }
  else {
  output_low(LED_PRESSAO_BAIXA);
  }
  
  if (V_2 >= 150 && V_2 <= 180){
   output_high(LED_PRESSAO_NORMAL);
   fprintf(PORT3,"Pressao Normal\r");
  }
  else {
  output_low(LED_PRESSAO_NORMAL); 
  }
  
  if (V_2 >= 180){
   output_high(LED_PRESSAO_ALTA);
   fprintf(PORT3,"Pressao Alta\r");
  }
  else {
  output_low(LED_PRESSAO_ALTA); 
  }
  
  delay_ms(1000);
   }

}
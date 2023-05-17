/****************************************************************************
Centro de Tecnologia Microgenios
Programa: Diplay_7_seg_01
Placa: KIT PICGENIOS
Objetivo: este programa tem por função ler o canal AD0 e AD1 e escrever no lcd
Centro de Tecnologia Microgenios
Programa: Diplay_7_seg_01
Placa: KIT PICGENIOS
Objetivo: este programa tem por função ler o canal AD0 e AD1 e escrever no lcd
o valor de conversão
Cristal = 8MHz
*******************************************************************************
*/

// Lcd module connections
sbit LCD_RS at LATE2_bit;
sbit LCD_EN at LATE1_bit;
sbit LCD_D4 at LATD4_bit;
sbit LCD_D5 at LATD5_bit;
sbit LCD_D6 at LATD6_bit;
sbit LCD_D7 at LATD7_bit;

sbit LCD_RS_Direction at TRISE2_bit;
sbit LCD_EN_Direction at TRISE1_bit;
sbit LCD_D4_Direction at TRISD4_bit;
sbit LCD_D5_Direction at TRISD5_bit;
sbit LCD_D6_Direction at TRISD6_bit;
sbit LCD_D7_Direction at TRISD7_bit;
// End Lcd module connections


char texto[14];
unsigned long temp_res = 0;
unsigned long temp_res2 = 0;
unsigned long pot1;
unsigned long pot2;

void main() {

trisb = 0; //define portb como saida
trisd = 0; //define portd como saida

ADCON1 = 0b00001101; //habilita canal A/D0 e A/D1 do PIC
trisa=0b00001111; //define pinos como entrada

Lcd_Init();                        // Initialize Lcd
Lcd_Cmd(_LCD_CLEAR); //apaga lcd
Lcd_Cmd(_LCD_CURSOR_OFF); //desliga cursor do lcd
Lcd_Out(1, 1, "AN0:"); //escreve mansagem na linha 1, coluna 1 do lcd

Lcd_Out(2, 1, "AN1:"); //escreve mensagem na linha 2, coluna 1 do lcd

ADCON1 = 0b00001011; //habilita canal A/D0 e A/D1 do PIC; VREF+= +5V; VREF- =GND
trisa=0b00001111; //define pinos como entrada
do
{
temp_res = Adc_Read(0); //le canal ad0 do PIC e salva valor na variável temp_res
delay_us(10); //delay de 10 microsegundos

pot1= (temp_res)*5000/1024;

temp_res2 = Adc_Read(1); //lê canal ad1 do PIC e salva valor na variável temp_res2
delay_us(10); //delay de 10 microsegundos

pot2= (temp_res2)*5000/1024;;

LongToStr(pot1, texto);
Lcd_out(1,6,texto); //escreve no lcd o valor da conversão do ad0

LongToStr(pot2, texto);
Lcd_out(2,6,texto); //escreve no lcd o valor da conversão do ad1



}
while (1);
}
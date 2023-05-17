#line 1 "C:/Microcontroladores PIC/PICGenios/ADC/ADC_PIC18F452.c"
#line 16 "C:/Microcontroladores PIC/PICGenios/ADC/ADC_PIC18F452.c"
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



char texto[14];
unsigned long temp_res = 0;
unsigned long temp_res2 = 0;
unsigned long pot1;
unsigned long pot2;

void main() {

trisb = 0;
trisd = 0;

ADCON1 = 0b00001101;
trisa=0b00001111;

Lcd_Init();
Lcd_Cmd(_LCD_CLEAR);
Lcd_Cmd(_LCD_CURSOR_OFF);
Lcd_Out(1, 1, "AN0:");

Lcd_Out(2, 1, "AN1:");

ADCON1 = 0b00001011;
trisa=0b00001111;
do
{
temp_res = Adc_Read(0);
delay_us(10);

pot1= (temp_res)*5000/1024;

temp_res2 = Adc_Read(1);
delay_us(10);

pot2= (temp_res2)*5000/1024;;

LongToStr(pot1, texto);
Lcd_out(1,6,texto);

LongToStr(pot2, texto);
Lcd_out(2,6,texto);



}
while (1);
}

#include <main.h>
#include <lcd.c>

void main()
{

  lcd_init();
	lcd_putc("\fPartida...");
	delay_ms(1000);
	lcd_putc("\f");

   while(TRUE)
   {
	 lcd_putc("\a");
	 
   if( input(pin_a0)){
   output_high(PIN_B0);
   lcd_putc("Chave1=   ligada\n\r");
   }
   else {
   output_low(PIN_B0);
   lcd_putc("Chave1=desligada\n\r");
   }

   if( input(pin_a1)){
   output_high(PIN_B1);
   lcd_putc("Chave2=   ligada");
   }   
   else{
   output_low(PIN_B1);
   lcd_putc("Chave2=desligada");
   }
       

   }

}

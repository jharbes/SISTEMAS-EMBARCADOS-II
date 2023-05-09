#include <main.h>

//#define use_portd_kbd TRUE // default
#include <KBD.C>

#define LCD_DATA_PORT getenv("SFR:PORTB")   
#include <lcd.c>


void main()
{
	char k;
 	
	int N1=1;
	int N2=2;
	int N3=3;
	int N4=4;	
	int N5=5;
 	
 	
	int j;
	int i;

	setup_adc_ports(NO_ANALOGS, VSS_VDD);
	kbd_init();

  lcd_init();

	lcd_putc("\fDigite uma tecla\n");

//!	while(TRUE)
//!	{
//!
//!		//Example using external LCD
//!		k = kbd_getc();
//!		if(k!=0)
//!			if(k=='*')
//!				lcd_putc('\f');
//!			else
//!				lcd_putc(k);
//!
//!	}
//!
//!}







while(TRUE)                     //Começo do programa
    {

           while(j == 0)
            {
             printf (lcd_putc, "\p\f   IPETEC 2021  "  );
             printf (lcd_putc, "\n DIGITE A SENHA "  );


						i = kbd_getc();
					 	
					 	
            }
            //BIP();
            printf (lcd_putc, "\f      SENHA      ");
            printf (lcd_putc, "\n      *");
            d_1 = i;
            delay_ms(500);
            j = 1;

            while(j == 1)
            {
                printf (lcd_putc, "\f      SENHA      ");
                printf (lcd_putc, "\n      *");
                i = kbd_getc();
            }
            BIP();
            printf (lcd_putc, "\f      SENHA      ");
            printf (lcd_putc, "\n      **");
            d_2 = i;
            delay_ms(500);
            j = 2;

            while(j == 2)
            {
                printf (lcd_putc, "\f      SENHA      ");
                printf (lcd_putc, "\n      **");
                i = kbd_getc();
            }
            BIP();
            printf (lcd_putc, "\f      SENHA      ");
            printf (lcd_putc, "\n      ***");
            d_3 = i;
            delay_ms(500);
            j = 3;

            while(j == 3)
            {
                printf (lcd_putc, "\f      SENHA      ");
                printf (lcd_putc, "\n      ***");
                i = kbd_getc();;
            }
            BIP();
            printf (lcd_putc, "\f      SENHA      ");
            printf (lcd_putc, "\n      ****");
            d_4 = i;
            delay_ms(500);
            j = 4;

            while(j == 4)
            {
                printf (lcd_putc, "\f      SENHA      ");
                printf (lcd_putc, "\n      ****");
                i = kbd_getc();
            }
            BIP();
            printf (lcd_putc, "\f      SENHA      ");
            printf (lcd_putc, "\n      *****");
            d_5 = i;
            delay_ms(500);
            j = 5;

            if( j == 5 )
            {
                if(d_1 == N1 && d_2 == N2 && d_3 == N3 && d_4 == N4 && d_5 ==N5 )
                {
                    printf (lcd_putc, "\fBem vindo "nome da pessoa" ");
                    printf (lcd_putc, "\n  Senha correta ");
                    //ABRIR();
                    delay_ms(3000);
                    i = 0;
                    j = 0;
                }
                
                                    
                else
                {
                           printf (lcd_putc, "\fSenha incorreta");
                           printf (lcd_putc, "\nTente novamente");
                           BIP();
                           delay_ms(100);
                           //BIP();
                           delay_ms(100);
                           //BIP();
                           delay_ms(100);
                           //BIP();
                           delay_ms(100);
                           //BIP();
                           delay_ms(100);
                           //BIP();
                           delay_ms(3000);
                           i = 0;
                           j = 0;
                }
                                    d_1 = 0;
                                    d_2 = 0;
                                    d_3 = 0;
                                    d_4 = 0;
                                    d_5 = 0;
                                
    }
}
    


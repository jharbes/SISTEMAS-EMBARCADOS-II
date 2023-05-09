// Program FONT.C
//
// Sequentially outputs ASCII characters to LCD
//
// copyright, Peter H. Anderson, Baltimore, MD, Dec, '00


#case

#device PIC16F877 *=16 ICD=TRUE

#include <defs_877.h>
#include <lcd_out.h>

void main(void)
{
    byte n;


    pcfg3 = 0; pcfg3 = 1; pcfg2 = 0; pcfg0 = 0;
            // configure A/D for 3/0 operation
            // this is necessary to use PORTE2::0 for the LCD
    lcd_init();

    for (n=0; ; n++)
    {

       lcd_clr_line(0);       // beginning of line 0
       printf(lcd_char, "%u  %x   %c", n, n, n);
       delay_ms(2000);
    }

}
#include <lcd_out.c>

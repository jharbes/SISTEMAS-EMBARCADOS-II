// Program LCD_TST.C
//
// Illustrates the various features of the routines in LCD_OUT.C
//
// copyright, Peter H. Anderson, Baltimore, MD, Dec, '00


#case

#device PIC16F877 *=16 ICD=TRUE

#include <defs_877.h>
#include <lcd_out.h>

void main(void)
{
    byte q, T_F_whole, T_F_fract;
    float T_F;
    long temp;

    pcfg3 = 0; pcfg3 = 1; pcfg2 = 0; pcfg0 = 0;
            // configure A/D for 3/0 operation
            // this is necessary to use PORTE2::0 for the LCD
    lcd_init();
    q = 0;

    while(1)
    {
       lcd_clr_line(0);       // beginning of line 0
       lcd_dec_byte(q, 3);
       lcd_cursor_pos(0, 10); // line 0, position 10
       lcd_hex_byte(q);

       lcd_clr_line(1);       // advance to line 1
       printf(lcd_char, "   Hello World   ");

       T_F = 76.6  + 0.015 * ((float) (q));
       lcd_clr_line(2);
       printf(lcd_char, "T_F = %f", T_F); // print a float

       lcd_clr_line(3);       // to last line
       printf(lcd_char, "T_F = ");

       temp = (long)(10.0 * T_F);   // separate T_F into two bytes
       T_F_whole = (byte)(temp/10);
       T_F_fract = (byte)(temp%10);

       if (T_F_whole > 99)    // leading zero suppression
       {
          lcd_dec_byte(T_F_whole, 3);
       }
       else if (T_F_whole > 9)
       {
          lcd_dec_byte(T_F_whole, 2);
       }
       else
       {
          lcd_dec_byte(T_F_whole, 1);
       }

       lcd_char('.');
       lcd_dec_byte(T_F_fract, 1);

       ++q;       // dummy up some new numbers

       delay_ms(1000);
   }
}

#include <lcd_out.c>

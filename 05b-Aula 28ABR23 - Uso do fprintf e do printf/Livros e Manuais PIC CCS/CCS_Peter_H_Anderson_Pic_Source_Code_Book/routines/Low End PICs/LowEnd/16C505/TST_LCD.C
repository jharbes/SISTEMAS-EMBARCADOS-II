// Program TST_LCD.C (PIC16C505), CCS, PCB
//
// Direct Interface with Optrex DMC20434.
//
// Illustrates the various features of the routines in LCD_OUT.C
//
// Displays a string, byte in decimal format and in hex format.
//
// copyright, Peter H. Anderson, Brattleboro, VT, July, '01

#case

#device PIC16C505 *=8

#include <defs_505.h>
#include <lcd_out.h>
#include <delay.h>

void main(void)
{
    byte n, q;
    byte const str_const[20] = {"   Hello World"};

    DIRC = 0x3f;
    DIRB = 0x3f;

    lcd_init();
    q = 0;
    while(1)
    {

       lcd_clr_line(0);       // beginning of line 0
       n=0;
       while (str_const[n])
       {
          lcd_char(str_const[n]);
          ++n;
       }
       lcd_clr_line(1);
       lcd_dec_byte(q, 3);
       lcd_cursor_pos(1, 10); // line 1, position 10
       lcd_hex_byte(q);

       ++q;  // modify the displayed quantity

       delay_ms(1000);
   }
}

#include <lcd_out.c>
#include <delay.c>

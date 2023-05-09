// Program TOGGLE_1.C
//
// Reverses the state of an LED on PORTD.4 when pushbutton on input PORTB.0 is
// momentarily depressed.  Also, continually outputs to the LCD.
//
// Note that there is a problem with switch bounce where an even number of
// bounces will cause an even number of toggles and thus the LED will not appear
// to change
//
// copyright, Peter H. Anderson, Baltimore, MD, Dec, '00


#case

#device PIC16F877 *=16 ICD=TRUE

#include <defs_877.h>
#include <lcd_out.h>

void main(void)
{
    byte n;

    pspmode = 0;  // PORTD as general purpose IO

    portd4 = 0;   // be sure LED is off
    trisd4 = 0;   // make it an output

    trisb0 = 1;   // make an input (not really neccessary)

    not_rbpu = 0; // enable weak pullup resistors on PORTB
    intedg = 0;   // interrupt on falling edge

    intf = 0;     // kill any unwanted interrupt
    inte = 1;     // enable external interrupt

    gie = 1;      // enable all interrupts

    pcfg3 = 0; pcfg3 = 1; pcfg2 = 0; pcfg0 = 0;
            // configure A/D for 3/0 operation
            // this is necessary to use PORTE2::0 for the LCD
    lcd_init();

    for (n=0; ; n++)    // continually
    {
       lcd_clr_line(0);       // beginning of line 0
       printf(lcd_char, "%u  %x   %c", n, n, n);
       delay_ms(2000);
    }

}

#int_ext ext_int_handler(void)
{
    portd4 = !portd4;   // invert the state of output
}

#int_default default_int_handler(void)
{
}

#include <lcd_out.c>

// Program PWM_1.C
//
// Illustrates use of CCP1 to implement 8-bit PWM on RC2/CCP1.
//
// When pushbutton is open (released), duty cycle decrease to zero.  When
// pushbutton is depressed, duty cycle slowly increases to the maximum of
// 255.
//
//
// copyright, Peter H. Anderson, Baltimore, MD, Dec, '00

#case

#device PIC16F877 *=16 ICD=TRUE

#include <defs_877.h>
#include <lcd_out.h> // LCD and delay routines

main()
{

   byte duty;

   not_rbpu = 0;  // enable weak pullups
   trisb0 = 1;

   PR2 = 0xff;    // period set to max of 256 usecs  - about 4 kHz
   duty = 0x00;
   CCPR1L = duty;  // duty initially set to zero

   // configure CCP1 for PWM operation
   ccp1m3 = 1;  ccp1m2 = 1;

   // Timer 2 post scale set to 1:1
   toutps3 = 0;  toutps2 = 0; toutps1 = 0; toutps0 = 0;

   // Timer 2 prescale set to 1:1
   t2ckps1 = 0; t2ckps0 = 0;

   tmr2on = 1; // turn on timer #2

   portc2 = 0;
   trisc2 = 0; // make PORTC.2 an output 0

   pcfg3 = 0; pcfg2 = 1; pcfg1 = 0; pcfg0 = 0;
   // config A/D for 3/0

   lcd_init();

   while(1)
   {
       if (rb0)   // go down
       {
           if (duty < 5)
           {
               duty = 0;
           }
           else
           {
               duty = duty - 5;
           }
       }
       else // increase duty
       {
           if (duty > (0xff - 5))
           {
               duty = 0xff;   // max
           }
           else
           {
               duty = duty + 5;
           }
       }
       CCPR1L = duty;
       lcd_cursor_pos(0, 0);
       lcd_hex_byte(duty);
       delay_ms(25);
   }
}

#include <lcd_out.c>

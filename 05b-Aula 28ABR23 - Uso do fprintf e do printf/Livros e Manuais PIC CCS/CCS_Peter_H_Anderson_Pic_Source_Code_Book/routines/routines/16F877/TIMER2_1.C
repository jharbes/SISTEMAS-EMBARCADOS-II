// Program TIMER2_1.C
//
// Generates nominal 500 Hz tone on PORTD.0 and performs continual A/D
// conversions on Channel 0 when push button on PORTB.0 is depressed.
//
// Illustrates use of TIMER2.
//
// copyright, Peter H. Anderson, Baltimore, MD, Dec, '00

#case

#device PIC16F877 *=16 ICD=TRUE

#include <defs_877.h>
#include <lcd_out.h> // LCD and delay routines

main()
{

   byte duty;
   long ad_val;

   pcfg3 = 0; pcfg2 = 1; pcfg1 = 0; pcfg0 = 0;
   // config A/D for 3/0

   lcd_init();

   not_rbpu = 0;  // enable weak pullups
   trisb0 = 1;

   pspmode = 0;
   portd0 = 0;    // make speaker an ouput 0
   trisd0 = 0;

   // Set up timer2
   PR2 = 250;    // period set to 250 * 4 usecs  = 1 ms

   // Timer 2 post scale set to 1:1
   toutps3 = 0;  toutps2 = 0; toutps1 = 0; toutps0 = 0;

   // Timer 2 prescale set to 1:4
   t2ckps1 = 0; t2ckps0 = 1;

   // set up A/D
   adfm = 1;   // right justified
   adcs1 = 1; adcs0 = 1; // internal RC

   adon=1;  // turn on the A/D
   chs2=0;  chs1=0;   chs0=0;
   delay_10us(10);      // a brief delay

   while(1)
   {
       if (!rb0)
       {
           tmr2ie = 1;
           peie = 1;
           tmr2on = 1;
           gie = 1;

           adgo = 1;
           while(adgo)     ;
           ad_val = ADRESH;
           ad_val = ad_val << 8 | ADRESL;

           lcd_cursor_pos(0, 0);
           printf(lcd_char, "%ld  ", ad_val);
           delay_ms(25);
       }
       else // do nothing
       {
           while(gie)
           {
               gie = 0;
           }
           tmr2ie = 0;
           tmr2on = 0;
       }

   }
}

#int_timer2 timer2_int_handler(void)
{
    portd0 = !portd0;
}

#include <lcd_out.c>

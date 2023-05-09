// Program TIMER1_1.C
//
// Illustrates the use of Timer 1 with the external 32.768 kHz crystal T1OSC0
// and T1OSC1.
//
// Each second, briefly blips the speaker and displays the elapsed time in
// seconds and in hour:minute:sec format on the LCD.
//
// copyright, Peter H. Anderson, Baltimore, MD, Dec, '00

#case

#device PIC16F877 *=16 ICD=TRUE

#include <defs_877.h>
#include <lcd_out.h> // LCD and delay routines

#define TRUE !0
#define FALSE 0

struct TM
{
    byte hr;
    byte mi;
    byte se;
};

void blip_tone(void);
void increment_time(struct TM *t);

byte timer1_int_occ;    // note that this is global

main()
{

   byte duty;
   long elapsed_t;
   struct TM t;

   pcfg3 = 0; pcfg2 = 1; pcfg1 = 0; pcfg0 = 0;
   // config A/D for 3/0

   lcd_init();

   portd0 = 0;    // make speaker an ouput 0
   trisd0 = 0;

   // Set up timer2
   PR2 = 250;    // period set to 250 * 4 usecs  = 1 ms

   // Timer 2 post scale set to 1:1
   toutps3 = 0;  toutps2 = 0; toutps1 = 0; toutps0 = 0;

   // Timer 2 prescale set to 1:4
   t2ckps1 = 0; t2ckps0 = 1;

   // Set up timer1
   t1oscen = 1;   // enable external crystal osc circuitry
   tmr1cs = 1;    // select this as the source

   t1ckps1 = 0;   t1ckps0 = 0;   // prescale of 1

   tmr1if = 0;    // kill any junk interrupt
   TMR1L = 0x00;
   TMR1L = 0x80;

   tmr1ie = 1;
   peie = 1;
   gie = 1;

   timer1_int_occ = FALSE;

   elapsed_t = 0;                // start with elapsed time = 0
   t.hr = 0;  t.mi = 0; t.se = 0;

   tmr1on = 1;

   lcd_clr_line(2);
   printf(lcd_char, "Impress the spouse");
   lcd_clr_line(3);
   printf(lcd_char, "with a personal msg!");

   while(1)
   {
       if (timer1_int_occ)
       {
           timer1_int_occ = FALSE;
           ++elapsed_t;
           increment_time(&t);
           lcd_clr_line(0);
           printf(lcd_char, "%ld    ", elapsed_t);

           lcd_clr_line(1);
      lcd_dec_byte(t.hr, 2);
           lcd_char(':');
           lcd_dec_byte(t.mi, 2);
           lcd_char(':');
           lcd_dec_byte(t.se, 2);
           blip_tone();
       }
       // else do nothing
   }
}

void blip_tone(void)
{
   tmr2ie = 1;    // turn on timer 2 and enable interrupts
   peie = 1;
   tmr2on = 1;
   gie = 1;

   delay_ms(200); // tone for nominally 200 ms

   tmr2ie = 0;
   tmr2on = 0;
}

void increment_time(struct TM *t)
{
    ++t->se;
    if (t->se > 59)
    {
        t->se = 0;
        ++t->mi;
        if (t->mi > 59)
        {
            t->mi = 0;
            ++t->hr;
            if (t->hr > 23)
            {
                t->hr = 0;
            }
        }
    }
}

#int_timer1 timer1_int_handler(void)
{
    timer1_int_occ = TRUE;
    TMR1H = 0x80;
}

#int_timer2 timer2_int_handler(void)
{
    portd0 = !portd0;
}

#include <lcd_out.c>

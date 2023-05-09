// Program TIMER1_1.C
//
// Illustrates the use of Timer 1 with the external 32.768 kHz crystal T1OSC0
// and T1OSC1.
//
// Each second, briefly blips the speaker and displays the elapsed time in
// seconds and in hour:minute:sec format on the terminal
//
// copyright, Peter H. Anderson, Baltimore, MD, Dec, '01

#case
#device PIC18C452

#include <defs_18c.h>
#include <delay.h>
#include <ser_18c.h>

#define FALSE 0
#define TRUE !0

#define MAKE_LONG(h, l)  (((long) h) << 8) | (l)

#define EXT_32KHZ

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
   // config A/D for 3/0 - not really necessary in this application

   ser_init();

   latb3 = 0;    // make speaker an ouput 0
   trisb3 = 0;

   // Set up timer2
   PR2 = 250;    // period set to 250 * 4 usecs  = 1 ms

   // Timer 2 post scale set to 1:1
   toutps3 = 0;  toutps2 = 0; toutps1 = 0; toutps0 = 0;

   // Timer 2 prescale set to 1:4
   t2ckps1 = 0; t2ckps0 = 1;		// thus, the rollover is 4 * 256us

   // Set up timer 1
#ifdef EXT_32KHZ
   t1oscen = 0;   // enable external crystal osc circuitry
#else
   t1oscen = 1;
#endif
   tmr1cs = 1;    // select external as the source


   t1ckps1 = 0;   t1ckps0 = 0;   // prescale of 1

   tmr1if = 0;    // kill any junk interrupt
   TMR1L = 0x00;
   TMR1H = 0x80;

   tmr1ie = 1;
   peie = 1;
   gieh = 1;

   timer1_int_occ = FALSE;

   elapsed_t = 0;                // start with elapsed time = 0
   t.hr = 0;  t.mi = 0; t.se = 0;

   tmr1on = 1;


   while(1)
   {
       if (timer1_int_occ)
       {
           timer1_int_occ = FALSE;
           ++elapsed_t;
           increment_time(&t);
           printf(ser_char, "%ld\t", elapsed_t);

           ser_dec_byte(t.mi, 2);
           ser_char(':');
           ser_dec_byte(t.se, 2);

           ser_newline();

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
   gieh = 1;

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

#int_timer1
timer1_int_handler(void)
{
    timer1_int_occ = TRUE;
    TMR1H = 0x80;
}

#int_timer2
timer2_int_handler(void)
{
    latb3 = !latb3;
}

#int_default
default_int_handler(void)
{
#asm
   NOP		// for debugging
   NOP
#endasm
}

#include <delay.c>
#include <ser_18c.c>

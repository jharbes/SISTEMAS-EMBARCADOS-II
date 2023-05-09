// Program THERM_2.C
//
// Illustrates the use of Timer 1 with the external 32.768 kHz crystal T1OSC0
// and T1OSC1.
//
// Each second, briefly blips the speaker and displays the elapsed time in
// seconds and in hour:minute:sec format on the LCD.  Also, every five seconds,
// performs an A/D conversion on A/D Ch 1 and displays the temperature in degrees
// C and F.  The result of the A/D conversion is also written to program EEPROM.
//
// After ten measurements, the data is read from EEPROM and displayed on the LCD.
//
// copyright, Peter H. Anderson, Baltimore, MD, Dec, '00


#case

#device PIC16F877 *=16 ICD=TRUE

#include <defs_877.h>
#include <lcd_out.h> // LCD and delay routines
#include <math.h>

#define TRUE !0
#define FALSE 0

#define THERM_A 0.0004132
#define THERM_B 0.000320135

// #define TEST

struct TM
{
    byte hr;
    byte mi;
    byte se;
};

long get_eeprom(long adr);
void put_eeprom(long adr, long dat);

float calc_T_C(long ad_val);
float T_C_to_T_F(float T_C);
long meas_ad1(void);

void blip_tone(void);
void increment_time(struct TM *t);

byte timer1_int_occ;    // note that this is global

main()
{

   byte num_secs, n, num_samples;
   long elapsed_t, ad_val;
   struct TM t;
   float T_C, T_F;

   pcfg3 = 0; pcfg2 = 1; pcfg1 = 0; pcfg0 = 0;
   // config A/D for 3/0

   lcd_init();

   pspmode = 0;
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

   num_secs = 0;
   num_samples = 0;

   while(1)
   {
       if (timer1_int_occ)
       {

           timer1_int_occ = FALSE;
           ++elapsed_t;
           increment_time(&t);
           lcd_cursor_pos(0, 0);
           printf(lcd_char, "%ld    ", elapsed_t);

           lcd_cursor_pos(1, 0);
           lcd_dec_byte(t.hr, 2);
           lcd_char(':');
           lcd_dec_byte(t.mi, 2);
           lcd_char(':');
           lcd_dec_byte(t.se, 2);

           ++num_secs;

           if (num_secs == 5)
           {
               num_secs = 0;
               blip_tone();

               ad_val = meas_ad1();

               if (ad_val == 0)
               {
                   ad_val = 1;      // avoid a divide by zero error
               }

               T_C = calc_T_C(ad_val);
               T_F = T_C_to_T_F(T_C);  // convert T_C to T_F

               lcd_cursor_pos(2, 0);
               printf(lcd_char, "T_C = %3.1f  ", T_C);
               lcd_cursor_pos(3, 0);
               printf(lcd_char, "T_F = %3.1f  ", T_F);

               put_eeprom(0x1000 + num_samples, ad_val);

               ++num_samples;
               if (num_samples == 10)
               {
                   break;
               }

           }
       }
       // else do nothing
   }
   // now dump the data

   delay_ms(500);
   lcd_init();

   printf(lcd_char, "Dumping Data");
   for (n=0; n<5; n++)
   {
       blip_tone();
       delay_ms(200);
   }

   lcd_clr();

   for (num_samples = 0; num_samples < 10; ++num_samples)
   {
       ad_val = get_eeprom(0x1000 + num_samples);
       if (ad_val == 0)
       {
           ad_val = 1;      // avoid a divide by zero error
       }

       T_C = calc_T_C(ad_val);

       lcd_cursor_pos(0, 0);
       printf(lcd_char, "%2d %3.1f  ", num_samples, T_C);
       delay_ms(1000);
   }

   lcd_clr_line(0);
   printf(lcd_char, "Done");
   delay_ms(1000);
   lcd_init();
   while(1)    // continual loop
   {
#asm
       CLRWDT
#endasm
   }
}

void put_eeprom(long adr, long dat)
{
    while(gie)       // be sure interrupts are disabled
    {
        gie = 0;
    }
    EEADRH = adr >> 8;
    EEADR = adr & 0xff;

    EEDATH = dat >> 8;
    EEDATA = dat & 0xff;
    eepgd = 1;    // program memory
    wren = 1;
    EECON2 = 0x55;
    EECON2 = 0xaa;
    wr = 1;
#asm
    NOP
    NOP
#endasm
    wren = 0;
    gie = 1;
}

long get_eeprom(long adr)
{
    long eeprom_val;
    EEADRH = adr >> 8;
    EEADR = adr & 0xff;
    eepgd = 1;
    rd = 1;
#asm
    NOP
    NOP
#endasm
    eeprom_val = EEDATH;
    eeprom_val = eeprom_val << 8 | EEDATA;
    return(eeprom_val);
}

float calc_T_C(long ad_val)
{
    float ad_val_float, r_therm, T_K, T_C;

    ad_val_float = (float) ad_val;
    r_therm = 10.0e3 / (1024.0/ad_val_float - 1.0);
    T_K = 1.0 / (THERM_A + THERM_B * log(r_therm));
    T_C = T_K - 273.15;
    return(T_C);
}

float T_C_to_T_F(float T_C)
{
    float T_F;
    T_F = T_C * 1.8 + 32.0;
    return(T_F);
}

long meas_ad1(void)
{
    long ad_val;

    adfm = 1;                 // right justified
    adcs1 = 1; adcs0 = 1;     // internal RC

    adon=1;                   // turn on the A/D
    chs2=0;  chs1=0;   chs0=1;   // channel 1
    delay_10us(10);      // a brief delay

    adgo = 1;
    while(adgo)    ;  // poll adgo until zero
    ad_val = ADRESH;
    ad_val = ad_val << 8 | ADRESL;
    adon = 0;
    return(ad_val);
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

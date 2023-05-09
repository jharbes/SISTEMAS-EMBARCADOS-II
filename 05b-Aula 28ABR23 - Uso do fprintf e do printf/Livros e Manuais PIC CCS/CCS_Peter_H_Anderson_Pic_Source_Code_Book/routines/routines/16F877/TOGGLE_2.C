// Program TOGGLE_2.C
//
// Illustrates the use of the A/D using interrupts.  Continually measures
// voltage on potentiometer on AN0 and displays A/D value and angle.
//
// Also toggles LED on PORTD.0 when pushbutton on PORTB.0 is depressed.
//
// copyright, Peter H. Anderson, Baltimore, MD, Dec, '00

#case

#device PIC16F877 *=16 ICD=TRUE

#include <defs_877.h>
#include <lcd_out.h> // LCD and delay routines

main()
{

   long ad_val;
   float angle;

   pcfg3 = 0; pcfg2 = 1; pcfg1 = 0; pcfg0 = 0;
   // config A/D for 3/0

   lcd_init();

   adfm = 1;   // right justified
   adcs1 = 1; adcs0 = 1; // internal RC

   adon=1;  // turn on the A/D
   chs2=0;  chs1=0;   chs0=0;
   delay_10us(10);   // brief delay to allow capture

   not_rbpu = 0;     // internal pullup enabled
   intedg = 0;       // negative going transition

   trisb0 = 1;

   pspmode = 0;
   portd4 = 0;       // start with LED off
   trisd4 = 0;

   gie = 1;

   while(1)
   {
      inte = 0;      // disable external interrupt

      adif = 0;      // kill any previous interrupt
      adie = 1;      // enable A/D interrupt
      peie = 1;      // enable peripherals

      adgo = 1;
#asm
      CLRWDT
      SLEEP
#endasm
      adie = 0;      // disable A/D interrupts

      intf = 0;      // kill any noise
      inte = 1;      // and enable external interrupt

      ad_val = ADRESH;
      ad_val = ad_val << 8 | ADRESL;
      angle = (float) ad_val * 270.0 / 1024.0;
      lcd_clr_line(0);
      printf(lcd_char, "%ld", ad_val);
      lcd_clr_line(1);
      printf(lcd_char, "Angle = %2.1f", angle);
      delay_ms(3000);   // three second delay
   }
}

#int_ad ad_int_handler(void)
{
}

#int_ext external_int_handler(void)
{
    portd4 = !portd4;
}

#int_default default_int_handler(void)
{
}

#include <lcd_out.c>

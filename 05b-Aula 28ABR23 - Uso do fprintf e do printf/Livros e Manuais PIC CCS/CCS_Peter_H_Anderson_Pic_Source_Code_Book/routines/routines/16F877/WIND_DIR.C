// Program WIND_DIR.C
//
// Uses Fascinating Electronics dual wiper potentiometer to determine wind
// direction (0 - 359).  Displays reading on LCD
//
//  Dual Wiper Pot				PIC
//    +5 -- term 1
//    Wiper 1 (term 2) ----------> RA1/AN1
//    Wiper 2 (term 3) ----------> RA3/AN3
//    GRD -- term 4				Note 100K pulldown resistors to GRD on RA1 and RA3
//
// copyright, Peter H. Anderson, Baltimore, MD, May, '01

#case

#device PIC16F877 *=16 ICD=TRUE

#include <defs_877.h>
#include <lcd_out.h> // LCD and delay routines

#define TRUE !0
#define FALSE 0

#define FUDGE 36
long meas_wind_dir(void);
float ad_meas(byte channel, byte num_samps);

void main(void)
{
 long angle;

 lcd_init();

 while(1)
 {
    angle = meas_wind_dir(); // measure the angle
    lcd_clr_line(0);   // and display with leading zero suppression
    if (angle > 99)
    {
      lcd_dec_byte(angle / 100, 1);
      lcd_dec_byte(angle % 100, 2);
    }
    else if (angle > 9)
    {
      lcd_dec_byte((byte) angle, 2);
    }
    else
    {
      lcd_dec_byte((byte) angle, 1);
    }
    delay_ms(200);   // with a brief delay
  }
}

long meas_wind_dir(void)
{
   float adval_float, angle_float;
   long angle;

   adval_float = ad_meas(1, 100); // avg of 100 meas on Ch 1
   if (adval_float > 100.0)	// if wiper #1 not in dead zone
   {
     angle_float = 0.2636 * adval_float;
   }
   else		// otherwise, use wiper #2
   {
     adval_float = ad_meas(3, 100);	// avg of 100 meas on Ch 3
     angle_float = 0.2636 * adval_float + 180.0;
   }
   angle = (long) angle_float;
   angle = angle + FUDGE;	// see text
   angle = angle % 360;
   return(angle);
}

float ad_meas(byte channel, byte num_samps)
{
   byte n;
   long adval;
   float sum = 0.0;

   pcfg3 = 0;  pcfg2 = 1;  pcfg1 = 0; pcfg0 = 0; // 3/0 configuration
   adfm = 1;

   adcs1 = 1;  adcs0 = 1;
   adon = 1;
   chs2 = 0;

   switch (channel)
   {
      case 1:  chs1 = 0; chs0 = 1;
           break;
      case 3:  chs1 = 1; chs0 = 1;
           break;
      default:
           return(-99.9);
   }

   for (n = 0; n<num_samps; n++)
   {
      adgo = 1;
      while(adgo)               ;
      adval = ADRESH;
      adval = (adval << 8) + ADRESL;
      sum = sum + (float) adval;
   }
   return(sum/(float) num_samps);
}

#include <lcd_out.c>

// barom_1.c (PIC16F877)
//
// Measures atmospheric pressure using an MPX4115A pressure sensor using an ADC
// on the PIC.  Calculates pressure in millibars and then calculates barometric
// pressure based on the altitude.  Displays in millibars and in inches of mercury.
//
// MPX4115A							PIC16F877
//
// Out (term 1) -------------------> AN0 (term 2)
// GRD (term 2)
// +5VDC (term 3)
//
// copyright, Peter H Anderson, Baltimore, MD, May, '01

#case

#device PIC16F877 *=16 ICD=TRUE

#include <defs_877.h>
#include <math.h>
#include <lcd_out.h> // LCD and delay routines

#define ELEVATION_METERS 133.0
#define FUDGE -0.2

float meas_pressure(void);
float adjust_pressure(float pressure, float alt_meters);
float power(float a, float b);

main()
{
   float atmos_pressure_mb, barom_pressure_mb, barom_pressure_Hg_in;

   pcfg3 = 0; pcfg2 = 1; pcfg1 = 0; pcfg0 = 0;
   // config A/D for 3/0

   lcd_init();

   while(1)
   {
      atmos_pressure_mb = meas_pressure();
      barom_pressure_mb = adjust_pressure(atmos_pressure_mb, ELEVATION_METERS);
      lcd_clr_line(0);
      printf(lcd_char, "Pressure = %3.1f", barom_pressure_mb);
      barom_pressure_Hg_in = 0.02953 * barom_pressure_mb + FUDGE;
      lcd_clr_line(1);
      printf(lcd_char, "Pressure = %3.2f", barom_pressure_Hg_in);

      delay_ms(5000);   // five second delay
   }
}

float meas_pressure(void)
{
   byte n;
   long ad_val;
   float sum = 0.0, ad_val_avg, atmos_pressure;

   adfm = 1;   // right justified
   adcs1 = 1; adcs0 = 1; // internal RC

   adon=1;  // turn on the A/D
   chs2=0;  chs1=0;   chs0=0;
   delay_10us(10);      // a brief delay

   for (n=0; n<100; n++)	// perform 100 A/D conversions and average
   {
      adgo = 1;
      while(adgo)    ;  // poll adgo until zero
      ad_val = ADRESH;
      ad_val = ad_val << 8 | ADRESL;
      sum = sum + (float) ad_val;
   }
   ad_val_avg = sum / 100.0;
   atmos_pressure = 1.085 * ad_val_avg + 105.56;
   return(atmos_pressure);
}

float adjust_pressure(float pressure, float alt_meters)
{
   float x, y, k;

   x = 2.255e-5 * alt_meters;
   x = 1.0 - x;
   y = power(x, 5.2558797);
   k = 1.0/y;
   return(k * pressure);
}

float power(float a, float b)
{
   float c;

   c = exp(b*log(a));
   return(c);
}

#include <lcd_out.c>


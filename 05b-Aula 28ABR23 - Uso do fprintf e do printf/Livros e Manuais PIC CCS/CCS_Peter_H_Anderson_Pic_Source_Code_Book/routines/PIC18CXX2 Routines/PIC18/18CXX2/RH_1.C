// Program RH_1.C
//
// Continually measures output of a Honeywell HIH-3605 and calculates and displays the
// relative humidity.
//
// A temperature measurement is also performed and the value of RH is corrected.  Note
// that an NTC thermistor in a voltage divider arrangement is used for measuring temperature.
//
//   HIH3605							 PIC18C452
//
//  GRD - Term 1
//  OUT - Term 2 ------------------------ AN1 (term 3)
//  +5 VDC - Term 3
//
//
//
//     +5 VDC
//	   |
//    10K
//	   |------------------- AN2 (term 4)
//     |
//    10K NTC Thermistor
//     |
//     GRD
//
// copyright, Peter H Anderson, Baltimore, MD, Nov, '01

#case
#device PIC18C452

#include <defs_18c.h>
#include <delay.h>
#include <ser_18c.h>

#include <math.h>

#define FALSE 0
#define TRUE !0

#define MAKE_LONG(h, l)  (((long) h) << 8) | (l)

#define  A_THERMISTOR  0.000623	// Thermistor two-point model used to calculate Temperature
#define  B_THERMISTOR  0.000297

float meas_RH(byte ad_channel);
float meas_T_C(byte ad_channel);

float mult_adc(byte ad_channel, long num_samps);

void main(void)
{

   float RH, T_C, RH_corrected;

   ser_init();

   while(1)
   {
      RH = meas_RH(1);		// meas relative humidity using A/D channel 0
      T_C = meas_T_C(2);
      RH_corrected = RH/(1.0546 - 0.00216 * T_C);

      printf(ser_char, "RH = %3.2f\tT_C = %3.2f\r\n", RH_corrected, T_C);

      delay_ms(5000);
   }
}


float meas_RH(byte ad_channel)
{

   float ad_val_avg, RH;

   ad_val_avg = mult_adc(ad_channel, 100);	// compute average of 100 samples
   RH = 0.157 * ad_val_avg - 25.80;
   return(RH);
}


float meas_T_C(byte ad_channel)
{
   float ad_val_avg, r_therm, T_K, T_C;

   ad_val_avg = mult_adc(ad_channel, 100);	// A/D conversion
   r_therm = 10000.0/(1024.0/ad_val_avg-1.0); // Calculate thermistor R
   T_K = 1.0/(A_THERMISTOR + B_THERMISTOR *log(r_therm));	// T_Kelvin
   T_C = T_K-273.15;					// T_Celcius

   return(T_C);
}


float mult_adc(byte ad_channel, long num_samps)
{
    float sum;
    long n, ad_val;

    pcfg3 = 0; pcfg2 = 1; pcfg1 = 0; pcfg0 = 0;
   // config A/D for 3/0

    adfm = 1;   // right justified
    adcs2 = 0; adcs1 = 1; adcs0 = 1; // internal RC

    adon=1;  // turn on the A/D

    chs2=0;

    switch(ad_channel)
    {
		case 0:    chs1=0;   chs0=0;
		           break;

        case 1:    chs1=0;   chs0=1;
                   break;

        case 2:    chs1=1;   chs0=0;
                   break;

        default:   break;
	}

    delay_10us(10);      // a brief delay

    sum = 0.0;

    for (n=0; n<num_samps; n++)
    {
		adgo = 1;
		while(adgo) /* wait */		;
		ad_val = MAKE_LONG(ADRESH, ADRESL);
        sum = sum + (float) ad_val;
	}

	return(sum/(float)num_samps);
}

#include <delay.c>
#include <ser_18c.c>

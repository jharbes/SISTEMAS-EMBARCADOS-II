// V_REF_2.C - PIC14000
//
//
// copyright, Peter H Anderson, Elmore, VT, June, '02

#case

#device PIC14000 *=16

#include <defs_14.h>
#include <delay.h>
#include <ser_14.h>

#define MAKE_LONG(h, l)  (((unsigned long) (h)) << 8) | (l)

#define TRUE !0
#define FALSE 0

#define TxData 2	// PORTC, Bit 2
#define INV

#define CERDIP2

#define AN1_IN 0x10
#define BG_IN 0x40
#define REFHI_IN 0x50
#define REFLO_IN 0x60
#define TEMP_IN 0x70
#define REFA 0x80
#define REFB 0x90

#define CURRENT28 0xb0

float set_vref(byte channel, float vref_val);
byte increment_vref_setting(byte x);
byte decrement_vref_setting(byte x);

float ad_meas(byte channel, byte current_source);

void display_calibration(void);

unsigned long ad_integrate(byte channel, byte current_source);
void read_calibration_float(unsigned long adr, float *p_float);

byte ad_int_occ, adov_int_occ, default_int_occ;	// global variables for interrupt

void main(void)
{
   float v;
   ser_init();
   display_calibration();

   while(1)
   {
      ser_char(0x0c);
      v = set_vref(REFA, 0.535);
      printf(ser_char, "REFA = %f", v);
      ser_new_line();
      v = set_vref(REFB, 0.40);
      printf(ser_char, "REFB = %f", v);
      ser_new_line();
      delay_ms(5000);
   }
}


float set_vref(byte channel, float vref_val)
{

	byte x, too_high, too_low, too_high_prev, too_low_prev;
   float v;

   cmoff = 0;


	if (vref_val > 0.55)
	{
		x = ((byte) ((vref_val - 0.55)/0.05) + 0x0a) * 8;
	}
	else if (vref_val < 0.45)
	{
		x = ((byte) ((0.45 - vref_val) / 0.05) + 0x0a) * 8 + 0x80;
	}

	else if (vref_val >=0.5)
	{
		x = ((byte) ((vref_val - 0.5) / 0.005)) * 8;
	}

	else
	{
		x = ((byte) ((0.5 - vref_val) / 0.005)) * 8 + 0x80;
	}


	if (channel == REFA)
	{
      cmaoe = 1;
		PREFA = x;
	}

	else
	{
      cmboe = 1;
		PREFB = x;
	}

	// do a measurement. if too small ++PREF, if too large --

	too_high_prev = FALSE;
	too_low_prev = FALSE;

	while(1)
	{

	   v = ad_meas(channel, CURRENT28);

	   if (v > vref_val)
	   {
		   x = decrement_vref_setting(x);
	   	   too_high = TRUE;
		   too_low = FALSE;
	   }
	   else
	   {
		   x = increment_vref_setting(x);
		   too_high = FALSE;
		   too_low = TRUE;
	   }

	   if ( ((too_high) && (too_low_prev)) || ((too_low) && (too_high_prev)) )
	   {
		   return(v);
	   }

	   else
	   {
		   too_high_prev = too_high;
		   too_low_prev = too_low;
	   }

      printf(ser_char, "%2x", x);
      ser_new_line();
      delay_ms(1000);

	   	if (channel == REFA)
	   	{
	   		PREFA = x;
	   	}

	   	else
	   	{
	   		PREFB = x;
	    }
   }
}

byte increment_vref_setting(byte x)
{
   byte y;

   y = x & 0x07;
   ++y;
   y = y & 0x07;
   x = (x & 0xf8) | y;

   if (y)			// if least sig three bits non zero
   {
	   return(x);
   }
   else				// if zero, then kick up the coarse adjustment
   {
      if (x == 0x80)
      {
         x = 0x00;
      }

      else if ((x & 0x80) == 0)
      {
         x = x + 8;
      }

      else
      {
         x = x - 8;
      }

      return(x);
   }
}

byte decrement_vref_setting(byte x)
{
   byte y;

   y = x & 0x07;	// strip off low three bits
   --y;				// decrement
   y = y & 0x07;
   x = (x & 0xf8) | y;
   if (y != 0x07)
   {

	   return(x);
   }

   else
   {

      if (x == 0x00)
      {
         x = 0x80;
      }

      else if ((x & 0x80) == 0)
      {
         x = x - 8;
      }

      else
      {
         x = x + 8;
      }
      return(x);
   }
}

float ad_meas(byte channel, byte current_source)
{
    unsigned long Nin, Nbg, Nrefhi, Nreflo;
    float Noffset, Vin, Kref, Kbg;

    read_calibration_float(0x0fc0, &Kref);
    read_calibration_float(0x0fc4,&Kbg);

    Nin = ad_integrate(channel, CURRENT28);
    Nbg = ad_integrate(BG_IN, CURRENT28);
    Nrefhi = ad_integrate(REFHI_IN, CURRENT28);
    Nreflo = ad_integrate(REFLO_IN, CURRENT28);

    Noffset = (float) Nreflo - Kref * (float) (Nrefhi - Nreflo);
    Vin = ((float) Nin - Noffset) / ( (float) Nbg - Noffset) * Kbg;
    return(Vin);
}

unsigned long ad_integrate(byte channel, byte current_source)
{
	unsigned long ret_val;

	refoff = 0;  adoff = 0; 	// turn on band gap reference and turn on A/D

    ADCON0 = (ADCON0 & 0x0f) | (channel);
    ADCON1 = (ADCON1 & 0x0f) | (current_source);

    pcfg3 = 0; pcfg2 = 0;	// portd configured as analog
    pcfg1 = 0; pcfg0 = 0;	// porta configured as analog

    amuxoe = 0;	// no output on AN0
    adzero = 0;
	adrst = 1;	// discharge the integrating cap - 200 us delay required
    delay_ms(1);

    ADTMRH = 0x00;
    ADTMRL = 0x00;

    ad_int_occ = FALSE;
    adov_int_occ = FALSE;
    default_int_occ = FALSE;

    ovfif = 0;  adcif = 0;	// kill any pending interrupts
    ovfie = 1; adcie = 1; peie = 1; gie = 1;

    adrst = 0;

    while(1)
    {
		if (ad_int_occ)
		{
			while(gie)
			{
				gie = 0;
			}
			adcif = 0;
			ret_val = MAKE_LONG(ADCAPH, ADCAPL);
			break;
		}

		if (adov_int_occ)
		{
			while(gie)
			{
				gie = 0;
			}
			ovfif = 0;
			ret_val = 0xffff;
			break;
		}

      if (default_int_occ)
      {
			while(gie)
			{
				gie = 0;
			}
			ret_val = 0x0000;
			break;
		}
	}
   peie = 0;  ovfie = 0;  adcie = 0;	// clean up
   adrst = 1;	// discharge
   return(ret_val);
}

void display_calibration(void)
{

    float Kref, Kbg, Vtherm, Ktc;

    lcd_cursor_pos(0, 0);
    printf(ser_char, "Calibration");

    lcd_cursor_pos(1, 0);	// fetch and display the calibration constants
    read_calibration_float(0x0fc0, &Kref);
    printf(ser_char, "%3.4f", Kref);

    lcd_cursor_pos(1, 10);
    read_calibration_float(0x0fc4,&Kbg);
    printf(ser_char, "%3.4f", Kbg);

    lcd_cursor_pos(2, 0);
    read_calibration_float(0x0fc8, &Vtherm);
    printf(ser_char, "%3.4f", Vtherm);

    lcd_cursor_pos(2, 10);
    read_calibration_float(0x0fcc, &Ktc);
    printf(ser_char, "%3.4f", Ktc);

    delay_ms(5000);
}

void read_calibration_float(unsigned long adr, float *p_float)
{
#ifdef CERDIP1
   const byte cal[16] = {0x7c, 0x00, 0x94, 0x5a, 0x7f, 0x18, 0x14, 0xa8,
                         0x7f, 0x0d, 0xf7, 0x55, 0x76, 0x68, 0xa6, 0xe4};

#endif

#ifdef CERDIP2
   const byte cal[16] = {0x7c, 0x01, 0xd4, 0x76, 0x7f, 0x1a, 0xd7, 0xae,
                         0x7f, 0x04, 0xf0, 0x3d, 0x76, 0x69, 0x0e, 0x5b};
#endif

#ifdef CERDIP3
   const byte cal[16] = {0x7c, 0x02, 0x17, 0xa5, 0x7f, 0x17, 0x52, 0xf6,
                         0x7f, 0x08, 0xc8, 0x42, 0x76, 0x77, 0xe6, 0x50};
#endif

   byte n, array[4];
#ifndef PRODUCTION

   for (n=0; n<4; n++)
   {
      array[n] = cal[adr - 0xfc0 + n];
   }
#else // Production

   for (n=0; n<4; n++)
   {
      array[n] = read_calibration(adr- 0xfc0 + n);	// fetch each byte in turn into a byte array
   }
#endif
   *p_float =  *((float *) array);	// copy the array to a float
}

#int_ad
ad_int_handler(void)
{
	ad_int_occ = TRUE;
}

#int_adof
adof_int_handler(void)
{
	adov_int_occ  = TRUE;
}

#int_default
default_int_handler(void)
{
   default_int_occ = TRUE;
}

#include <delay.c>
#include <ser_14.c>



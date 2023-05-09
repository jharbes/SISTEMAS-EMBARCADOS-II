// AD_MEAS1.C - PIC14000
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

#define CURRENT28 0xb0

unsigned long ad_integrate(byte channel, byte current_source);
void read_calibration_float(unsigned long adr, float *p_float);

byte ad_int_occ, adov_int_occ, default_int_occ;	// global variables for interrupt

void main(void)
{
	float Kref, Kbg, Vtherm, Ktc;	// calibration constants
	unsigned long Nin, Nbg, Nrefhi, Nreflo, Ntint;
	float Noffset;
	float V_an3, Vtint, T_c, T_f;

    ser_init();

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

    while(1)
    {
         ser_char(0x0c);
         printf(ser_char, "Hello");

         Nin = ad_integrate(AN1_IN, CURRENT28);
         Nbg = ad_integrate(BG_IN, CURRENT28);
         Nrefhi = ad_integrate(REFHI_IN, CURRENT28);
         Nreflo = ad_integrate(REFLO_IN, CURRENT28);
         Ntint = ad_integrate(TEMP_IN, CURRENT28);

         // now display these and wait for a bit

       lcd_cursor_pos(0, 0);
		 printf(ser_char, "Counts");

		 lcd_cursor_pos(1, 0);
		 printf(ser_char, "%lu",  Nin);

		 lcd_cursor_pos(1, 10);
		 printf(ser_char, "%lu",  Nbg);

		 lcd_cursor_pos(2, 0);
		 printf(ser_char, "%lu",  Nrefhi);

		 lcd_cursor_pos(2, 10);
		 printf(ser_char, "%lu",  Nreflo);

		 lcd_cursor_pos(3, 0);
		 printf(ser_char, "%lu", Ntint);

       delay_ms(5000);

         Noffset = (float) Nreflo - Kref * (float) (Nrefhi - Nreflo);
         V_an3 = ((float) Nin - Noffset) / ( (float) Nbg - Noffset) * Kbg;
         Vtint = ((float) Ntint - Noffset) / ( (float) Nbg - Noffset) * Kbg;
         T_c = (Vtint - Vtherm) / Ktc + 25.0;
         T_f = T_c * 1.8 + 32.0;

         // now display these and wait for a bit

         lcd_cursor_pos(0, 0);
		 printf(ser_char, "Values");

       lcd_cursor_pos(1, 0);
		 printf(ser_char, "%lu", (unsigned long) Noffset);

		 lcd_cursor_pos(2, 0);
		 printf(ser_char, "%4f", V_an3);

		 lcd_cursor_pos(2, 10);
		 printf(ser_char, "%4f", Vtint);

  		 lcd_cursor_pos(3, 0);
		 printf(ser_char, "%4f", T_c);

		 lcd_cursor_pos(3, 10);
		 printf(ser_char, "%4f", T_f);

       delay_ms(5000);
    }
}

unsigned long ad_integrate(byte channel, byte current_source)
{
	unsigned long ret_val;

	refoff = 0;  adoff = 0; tempoff = 0;	// turn on band gap reference and turn on A/D

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



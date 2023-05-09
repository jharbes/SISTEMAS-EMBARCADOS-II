// HUMID_2.C (PIC16F877)
//
// Illustrates an interface with a DS2438 Battery Monitor and
// a Honeywell HIH-3610 Humidity Sensor.
//
// Continually measures V_dd (Voltage source), V_ad (output of humidity
// sensor) and temperature T_C and calculates RH and RH corrected for
// temperature.
//
//  PIC16F877      DS2438
//
//  PORTD0 (term 19) ------------ DQ (term 8)
//
//     HIH-3610
//      			V_out ------ VAD (term 4)
//
// 4.7K pullup to +5 VDC on DQ.
//
// Note that a module consisting of the DS2438 and an HIH3610 on a small
// PCB is available from Dallas Semi as "DSHS01K Humidity Sensor Experimenter's
// Kit"
//
// copyright, Peter H. Anderson, Baltimore, MD, June, '01

#case
#device PIC16F877 *=16 ICD=TRUE

#include <a:\defs_877.h>
#include <a:\lcd_out.h>
#include <a:\_1_wire.h>

#define FALSE 0
#define TRUE !0

#define V_AD_SOURCE 1
#define V_DD_SOURCE 0

float meas_2438_T_C(void);
float meas_2438_V(byte source);

float calc_RH(float V_DD, float V_AD);
float calc_RH_temp_corrected(float RH, float T_C);

void main(void)
{
   float V_AD, V_DD, T_C, RH, RH_corrected;

   pspmode = 0;

   lcd_init();
   printf(lcd_char, "...............");  // to show that something is ging on

   while(1)
   {
      T_C = meas_2438_T_C( );

      V_AD = meas_2438_V(V_AD_SOURCE);
      V_DD = meas_2438_V(V_DD_SOURCE);

      RH = calc_RH(V_DD, V_AD);
      RH_corrected = calc_RH_temp_corrected(RH, T_C);

      lcd_clr_line(0);
      printf(lcd_char, "V_dd = %3.2f", V_DD);

      lcd_clr_line(1);
      printf(lcd_char, "V_ad = %3.2f", V_AD);

      lcd_clr_line(2);
      printf(lcd_char, "T_C = %3.2f", T_C);

      lcd_clr_line(3);
      printf(lcd_char, "RH = %3.1f", RH_corrected);

      delay_ms(5000);
  }
}

void meas_2438_T_C(void)
{
   byte a[8], n;
   signed long T_C_long;
   float T_C_float;

   _1w_init(0); // first set config byte
   _1w_out_byte(0, 0xcc);
   _1w_out_byte(0, 0x4e);
   _1w_out_byte(0, 0x00); // page 0
   _1w_out_byte(0, 0x00);


   _1w_init(0);
   _1w_out_byte(0, 0xcc);
   _1w_out_byte(0, 0x44); // temperature conversion
   delay_ms(1000);

   _1w_init(0);
   _1w_out_byte(0, 0xcc); // recall memory
   _1w_out_byte(0, 0xb8);
   _1w_out_byte(0, 0x00);

   _1w_init(0);   // send data
   _1w_out_byte(0, 0xcc);
   _1w_out_byte(0, 0xbe);
   _1w_out_byte(0, 0x00); // page 0

    for (n=0; n<9; n++)
    {
         a[n] = _1w_in_byte(0);
    }

    T_C_long = a[2];
    T_C_long = (T_C_long << 8) + a[1]; // put the two bytes together
    T_C_long = T_C_long / 8;   // see text
    T_C_float = (float) T_C_long * 0.03125;
    return(T_C_float);
}

float meas_2438_V(byte source)
{
   byte a[8], n;
   long ad_val;

   _1w_init(0); // first set config byte
   _1w_out_byte(0, 0xcc);
   _1w_out_byte(0, 0x4e);
   _1w_out_byte(0, 0x00); // page 0
   if (source == V_AD_SOURCE)
   {
      _1w_out_byte(0, 0x00);
   }
   else
   {
      _1w_out_byte(0, 0x08);
   }

   _1w_init(0);
   _1w_out_byte(0, 0xcc); // perform ADC
   _1w_out_byte(0, 0xb4);
   delay_ms(1);

   _1w_init(0);
   _1w_out_byte(0, 0xcc); // recall memory
   _1w_out_byte(0, 0xb8);
   _1w_out_byte(0, 0x00);

   _1w_init(0);   // send data
   _1w_out_byte(0, 0xcc);
   _1w_out_byte(0, 0xbe);
   _1w_out_byte(0, 0x00); // page 0

    for (n=0; n<9; n++)
    {
         a[n] = _1w_in_byte(0);
    }

    ad_val = a[4]; // high byte
 ad_val = (ad_val << 8) | a[3];
    return(0.01 * (float) ad_val);
}

float calc_RH(float V_DD, float V_AD)
{
   float RH;
   RH = ((V_AD / V_DD) - 0.16) / 0.0062;
   return(RH);
}

float calc_RH_temp_corrected(float RH, float T_C)
{
   float RH_corrected;
   RH_corrected = RH * (1.0546 - 0.00216 * T_C);
   return(RH_corrected);
}

#include <a:\lcd_out.c>
#include <a:\_1_wire.c>

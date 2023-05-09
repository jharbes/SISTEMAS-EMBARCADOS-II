// 2543_2.C
//
// Illustrates interfacing with a TI TLC2543 11-channel 12-bit A/D using the PIC's
// SSP module in the SPI mode.
//
// Performs a sequence of A/D measurements on specified channels (array channels[]) with
// specified polarities (array polarities[]).  The results are displayed on the LCD.
//
// This is an example of how the TLC2543 may be configured for the next A/D measurement
// while at the same time receiving the result of the previous command.
//
//
//	PIC16F877							TLC2543
//
// RC5/SDO (term 24) --------------- DIN (term 17)
// RC4/SDI (term 23) <-------------- DOUT (term 16)
// RC3/SCK (term 18) --------------- CLK (term 18)
// RB5 (term 38) ------------------> /CS (term 15
//
// Copyright, Peter H. Anderson, Baltimore, MD, Jan, '01

#case

#device PIC16F877 *=16 ICD=TRUE

#include <defs_877.h>
#include <lcd_out.h>

#define TRUE !0
#define FALSE 0


void mult_ad_meas(byte *channels, byte *polarities, long *ad_vals, byte num_channels);
void display(byte *channels, long *ad_vals, byte num_channels);

#define NUM_CHANNELS 4

void main(void)
{
  byte channels[NUM_CHANNELS] = {0, 1, 3, 4};
  byte polarities[NUM_CHANNELS] = {0, 1, 0, 0};
  long ad_vals[NUM_CHANNELS];

  lcd_init();
  while(1)
  {
     mult_ad_meas(channels, polarities, ad_vals, NUM_CHANNELS);
	  display(channels, ad_vals, NUM_CHANNELS);
     delay_ms(1000);
  }
}

void mult_ad_meas(byte *channels, byte *polarities, long *ad_vals, byte num_channels)
{
   byte high_byte, low_byte, m, n;
   unsigned long ad_val;
   sspen = 0;
   sspen = 1;
   sspm3 = 0; sspm2 = 0; sspm1 = 1; sspm0 = 0;	// Configure as SPI Master, fosc / 64
   ckp = 0; 							// idle state for clock is zero
   stat_cke = 1;						// data transmitted on rising edge
   stat_smp = 1; 						// input data sampled at end of clock pulse

   portc3 = 0;
   trisc3 = 0;	// SCK as output 0

   trisc4 = 1;	// SDI as input
   trisc5 = 0;	// SDO as output

   rb5 =1;
   trisb5 = 0;	// be sure CS is high

   for (n=0; n<num_channels+1; n++)
   {
	  rb5 = 0;		// CS low
      delay_10us(10);

	  if (n==num_channels)	// if it is the last meas
	  {
		   SSPBUF = 0x00;	// dummy
	  }
	  else
	  {
         SSPBUF = (channels[n] << 4) | (0x0c + polarities[n]);
      }

      while(!stat_bf)   /* loop */         ;
      high_byte = SSPBUF;

      SSPBUF = 0x00;	// send a dummy byte
      while(!stat_bf)   /* loop */           ;
      low_byte = SSPBUF;
      rb5 = 1;		// CS high

	  if (n!=0)	// if it is not the first
  	  {
          ad_val = (((unsigned long) high_byte) << 4) | (low_byte >> 4);
	      *ad_vals = ad_val;
          ++ad_vals;			// ******** See text *******
	  }
   }
}

void display(byte *channels, long *ad_vals, byte num_channels)
{
   byte n;

   for (n=0; n<num_channels; n++)
   {
	   lcd_clr_line(n);
	   printf(lcd_char, "%d %4lx", channels[n], *ad_vals);
       ++ad_vals;				// ******** See text ********
   }
}

#include <lcd_out.c>

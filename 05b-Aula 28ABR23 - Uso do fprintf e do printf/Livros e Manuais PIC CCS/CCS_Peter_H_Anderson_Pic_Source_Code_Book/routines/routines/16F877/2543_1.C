// 2543_1.C
//
// Illustrates interfacing with a TI TLC2543 11-channel 12-bit A/D using the PIC's
// SSP module in the SPI mode.
//
// Performs an A/D unipolar measurement on Channel 0 and a bipolar measurement on Ch 1
// and displays the result on the LCD.
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

unsigned long ad_meas(byte channel, byte bipolar);
void display(byte channel, unsigned long ad_val);

void main(void)
{
  byte channel;
  unsigned long ad_val;

  lcd_init();

  while(1)
  {
      channel = 0;
      ad_val = ad_meas(channel, 0);	// unipolar measurement on Ch 0
	  display(channel, ad_val);

	  channel = 1;
	  ad_val = ad_meas(channel, 1);	// bipolar measurement on Ch 1
	  display(channel, ad_val);

	  delay_ms(1000);
   }
}

unsigned long ad_meas(byte channel, byte bipolar)
{
	byte high_byte, low_byte;
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

    rb5 = 1;
    trisb5 = 0;	// be sure CS is high

       			// write the command
    rb5 = 0;		// bring CS low
    delay_10us(10);
    SSPBUF = (channel << 4) | (0x0c + bipolar);

    while(!stat_bf)   /* loop */         ;
    high_byte = SSPBUF;

    SSPBUF = 0x00;	// send a dummy byte
    while(!stat_bf)   /* loop */           ;
    low_byte = SSPBUF;
    rb5 = 1;		// CS high

					// now read the result
	rb5 = 0;		// bring CS low
	delay_10us(10);
	SSPBUF = 0x00;  // dummy

	while(!stat_bf)   /* loop */         ;
    high_byte = SSPBUF;

    SSPBUF = 0x00;	// send a dummy byte
    while(!stat_bf)   /* loop */           ;
    low_byte = SSPBUF;

    rb5 = 1;		// CS high

    ad_val = (((unsigned long) high_byte) << 4) | (low_byte >> 4);
    return(ad_val);
}

void display(byte channel, unsigned long ad_val)
{
   lcd_clr_line(channel);
   printf(lcd_char, "%d  %4lx", channel, ad_val);
}

#include <lcd_out.c>

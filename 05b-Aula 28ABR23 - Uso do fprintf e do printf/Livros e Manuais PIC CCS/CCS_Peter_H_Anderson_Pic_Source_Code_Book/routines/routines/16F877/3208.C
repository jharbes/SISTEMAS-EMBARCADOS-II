// 3208_1.C
//
// Illustrates interfacing with a Microchip MCP3208 8-channel 12-bit A/D.  Performs
// A/D measurments on Ch 0 and Ch 1 and differential measurements Ch0+ relative to Ch 1
// and Ch1+ relative to Ch 0.  Displays results to LCD.
//
// RC5/SDO (term 24) ------------->	DIN (11)
// RC4/SDI (term 23) <------------- DOUT (12)
// RC3/SCK (term 18) -------------> CLK (13)
// RB3 (term 36) -----------------> /CS (10)
//
// Copyright, Peter H. Anderson, Baltimore, MD, Jan, '01

#case

#device PIC16F877 *=16 ICD=TRUE

#include <defs_877.h>
#include <lcd_out.h>

#define TRUE !0
#define FALSE 0

unsigned long ad_meas_single_end(byte channel);
unsigned long ad_meas_diff(byte channel, byte polarity);

void display(byte line, unsigned long ad_val);

void main(void)
{
  byte channel;
  unsigned long ad_val;

  lcd_init();

  while(1)
  {
      ad_val = ad_meas_single_end(0); 		// single end measurement on Ch 0
	  display(0, ad_val);

	  ad_val = ad_meas_single_end(1);		// and Ch 1
	  display(1, ad_val);

	  ad_val = ad_meas_diff(0, 0);		// differential between Ch0 and Ch1 - Ch 0 more positive
	  display(2, ad_val);

	  ad_val = ad_meas_diff(0, 1);		// differential - Ch 1 more postive
	  display(3, ad_val);

	  delay_ms(3000);
   }
}

unsigned long ad_meas_single_end(byte channel)
{
	byte command, dummy, high_byte, low_byte;
	unsigned long ad_val;

	sspen = 0;
	sspen = 1;
	sspm3 = 0; sspm2 = 0; sspm1 = 1; sspm0 = 0;	// Configure as SPI Master, fosc / 64
	ckp = 1; 							// idle state for clock is zero
	stat_cke = 0;						// data transmitted on rising edge
	stat_smp = 1; 						// input data sampled at end of clock pulse

	portc3 = 1;
	trisc3 = 0;	// SCK as output 0

	trisc4 = 1;	// SDI as input
	trisc5 = 0;	// SDO as output

	rb3 =1;
	trisb1 = 0;	// be sure CS is high

	rb3 = 0;
	delay_10us(10);

	command = 0x04 + 0x02 + ((channel >> 2) & 0x01);

	SSPBUF = command;
	while(!stat_bf)   /* loop */         ;
    dummy = SSPBUF;

    command = (channel & 0x03) << 6;	// low two bits of channel

    SSPBUF = command;
	while(!stat_bf)   /* loop */         ;
    high_byte = SSPBUF;

    SSPBUF = 0x00;
    while(!stat_bf)   /* loop */         ;
    low_byte = SSPBUF;

    rb3 = 1;

	ad_val = high_byte & 0x0f;
	ad_val = (ad_val << 8) | low_byte;
	return(ad_val);
}

unsigned long ad_meas_diff(byte channel, byte polarity)
{
    byte command, dummy, high_byte, low_byte;
	unsigned long ad_val;

	sspen = 0;
	sspen = 1;
	sspm3 = 0; sspm2 = 0; sspm1 = 1; sspm0 = 0;	// Configure as SPI Master, fosc / 64
	ckp = 1; 							// idle state for clock is zero
	stat_cke = 0;						// data transmitted on rising edge
	stat_smp = 1; 						// input data sampled at end of clock pulse

	portc3 = 1;
	trisc3 = 0;	// SCK as output 0

	trisc4 = 1;	// SDI as input
	trisc5 = 0;	// SDO as output

	rb3 = 1;
	trisb3 = 0;	// be sure CS is high

	rb3 = 0;
	delay_10us(10);

	command = 0x04 + 0x00 + (channel >> 1);

	SSPBUF = command;
	while(!stat_bf)   /* loop */         ;
    dummy = SSPBUF;

    command = (((channel & 0x01) << 1) | polarity) << 6;	// low bit of channel and pol

    SSPBUF = command;
	while(!stat_bf)   /* loop */         ;
    high_byte = SSPBUF;

    SSPBUF = 0x00;
    while(!stat_bf)   /* loop */         ;
    low_byte = SSPBUF;

    rb3 = 1;

	ad_val = high_byte & 0x0f;
	ad_val = (ad_val << 8) | low_byte;
	return(ad_val);
}

void display(byte line, unsigned long ad_val)
{
   lcd_clr_line(line);
   printf(lcd_char, "%4lx", ad_val);
}

#include <lcd_out.c>


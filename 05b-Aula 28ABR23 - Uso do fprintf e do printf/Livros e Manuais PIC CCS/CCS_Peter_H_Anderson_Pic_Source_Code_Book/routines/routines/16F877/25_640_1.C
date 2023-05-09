// 25_640_1.C
//
// Illustrates the use of the SSP module in interfacing with a 25LC640
// EEPROM using the SPI protocol.  Illustrates single byte mode.
//
// Uses single byte write and read.  Writes 10 values beginning at location
// 0x0700 and then reads back the values and displays them on the LCD.
//
//	PIC16F877								25LC640
//
// RC5/SDO (term 24) -------------------> SI (term 5)
// RC4/SDI (term 23) <------------------- SO (term 2)
// RC3/SCK (term 18) -------------------> SCK (term 6)
// RB4/CS (term 37) --------------------> /CS (term 1)
//
//
// Copyright, Peter H. Anderson, Baltimore, MD, Jan, '01

#case

#device PIC16F877 *=16 ICD=TRUE

#include <defs_877.h>
#include <lcd_out.h>

#define TRUE !0
#define FALSE 0

#define SPI_WREN 0x06
#define SPI_WRITE 0x02
#define SPI_READ 0x03

void _25_640_setup_SPI(void);
void _25_640_write_byte(unsigned long adr, byte dat);
byte _25_640_read_byte(unsigned long adr);

void main(void)
{
   byte n, dat;
   unsigned long  adr;

   lcd_init();

   rb4 = 1;		// CS for 25LC640
   rb5 = 1;		// CS for TLC2543
   trisb4 = 0;
   trisb5 = 0;

   _25_640_setup_SPI();

   lcd_cursor_pos(0, 0);
   for (n=0, adr = 0x0700; n<10; n++, adr++)
   {
     _25_640_write_byte(adr, n+10);		// write 10, 11, 12, etc
     lcd_char('!');						// to see that something is happening
   }

  for (n=0, adr = 0x0700; n<10; n++, adr++)
  {
	 dat = _25_640_read_byte(adr);		// now read back the data
	 lcd_clr_line(1);
     lcd_dec_byte(dat, 2);     			// and display
     delay_ms(250);
  }

  while(1)		;						// continual loop
}

void _25_640_setup_SPI(void)
{
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
}

void _25_640_write_byte(unsigned long adr, byte dat)
{
    byte dummy;
    rb4 = 0;					// CS low

    SSPBUF = SPI_WREN;
    while(!stat_bf)   /* loop */         ;
    dummy = SSPBUF;

    rb4 = 1;					// CS high - end of WREN sequence

	rb4 = 0;					// begin another session - 4 bytes

	SSPBUF = SPI_WRITE;
	while(!stat_bf)   /* loop */         ;
    dummy = SSPBUF;

	SSPBUF = (byte) (adr >> 8);	// high byte of adr
	while(!stat_bf)   /* loop */         ;
    dummy = SSPBUF;

	SSPBUF = (byte) (adr);	// low byte of adr
	while(!stat_bf)   /* loop */         ;
    dummy = SSPBUF;

	SSPBUF = dat;	// data
	while(!stat_bf)   /* loop */         ;
    dummy = SSPBUF;

	rb4 = 1;
    delay_ms(5); 	// allow time for programming EEPROM
}

byte _25_640_read_byte(unsigned long adr)
{
    byte high_adr, low_adr, dummy, dat;

	rb4 = 0;		// CS low

	SSPBUF = SPI_READ;
	while(!stat_bf)   /* loop */         ;
    dummy = SSPBUF;

	high_adr = adr >> 8;

	SSPBUF = high_adr;	// high byte of adr
	while(!stat_bf)   /* loop */         ;
    dummy = SSPBUF;

	low_adr = adr;
	SSPBUF =  low_adr;	// low byte of adr
	while(!stat_bf)   /* loop */         ;
    dummy = SSPBUF;

	SSPBUF = dummy;
	while(!stat_bf)   /* loop */         ;
    dat = SSPBUF;
	rb4 = 1;

    return(dat);
}

#include <lcd_out.c>

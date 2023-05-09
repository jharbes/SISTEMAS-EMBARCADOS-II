// 25_640_2.C
//
// Illustrates the use of the SSP module in interfacing with a 25LC640
// EEPROM using the SPI protocol.  Illustrates block write and read mode.
//
// Writes one block of eight bytes beginning to locations beginning at
// 0x0700 and another block to 0x0708.  Then reads back the values and
// displays them on the LCD.
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
void _25_640_write_seq_bytes(unsigned long adr, byte *write_dat, byte num_bytes);
void _25_640_read_seq_bytes(unsigned long adr, byte *read_dat, byte num_bytes);

void display(byte *read_dat, byte num_bytes);

void main(void)
{
	byte dat_array[8], n;
	byte const a[8] = {0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00};
	byte const b[8] = {0x17, 0x16, 0x15, 0x14, 0x13, 0x12, 0x11, 0x10};

    lcd_init();

    rb4 = 1;		// CS for 25LC640
    rb5 = 1;		// CS for TLC2543
    trisb4 = 0;
    trisb5 = 0;

    _25_640_setup_SPI();

    for(n=0; n<8; n++)								// write a block of eight bytes
    {
		dat_array[n] = a[n];
	}

    _25_640_write_seq_bytes(0x700, dat_array, 8);

    for(n=0; n<8; n++)								// write another block
    {
		dat_array[n] = a[n];
	}

	_25_640_write_seq_bytes(0x708, dat_array, 8);  // write another


    _25_640_read_seq_bytes(0x700, dat_array, 8);  // read each block and display
	display(dat_array, 8);

	_25_640_read_seq_bytes(0x708, dat_array, 8);
	display(dat_array, 8);

	while(1)	;
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

void _25_640_write_seq_bytes(unsigned long adr, byte *write_dat, byte num_bytes)
{
    byte dummy, n;

	rb4 = 0;		// CS low - begin WREN sequence

	SSPBUF = SPI_WREN;
    while(!stat_bf)   /* loop */         ;
    dummy = SSPBUF;

	rb4 = 1;

	rb4 = 0;

	SSPBUF = SPI_WRITE;
	while(!stat_bf)   /* loop */         ;
    dummy = SSPBUF;

	SSPBUF = (byte) (adr >> 8);	// high byte of adr
	while(!stat_bf)   /* loop */         ;
    dummy = SSPBUF;

	SSPBUF = (byte) (adr);	// low byte of adr
	while(!stat_bf)   /* loop */         ;
    dummy = SSPBUF;

	for (n=0; n<num_bytes; n++)
	{

	   SSPBUF = write_dat[n];	// data
	   while(!stat_bf)   /* loop */         ;
       dummy = SSPBUF;
	}

	rb4 = 1;			// end of sequence
	delay_ms(5);  // allow time for programming
}

void _25_640_read_seq_bytes(int adr, byte *read_dat, byte num_bytes)
{
    byte high_adr, low_adr, dummy, n;

	rb4 = 0;		// CS low - begin sequence

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

	for (n=0; n<num_bytes; n++)
	{

	   SSPBUF = dummy;
	   while(!stat_bf)   /* loop */         ;
       read_dat[n] = SSPBUF;
	}
	rb4 = 1;		// end of sequence
}

void display(byte *read_dat, byte num_bytes)
{
    byte n, line = 0;
    lcd_clr_line(0);
	for (n=0; n<num_bytes; n++)
	{
	   lcd_hex_byte(read_dat[n]);
	   lcd_char(' ');

	   if (((n+1)%4) == 0)		// four values per line
	   {
		   ++line;
		   lcd_clr_line(line);
	   }
	}
}

#include <lcd_out.c>


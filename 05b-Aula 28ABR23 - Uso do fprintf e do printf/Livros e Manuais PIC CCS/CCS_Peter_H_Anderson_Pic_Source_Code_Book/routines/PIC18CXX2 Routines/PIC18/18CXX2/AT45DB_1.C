// AT45DB_1.C
//
// Illustrates the use of the SSP module in interfacing with an AT45DB321 Flash
// EEPROM using the SPI protocol.  Illustrates writing to and reading from RAM
// buffer 1.
//
//	PIC18F452
//
// RC5/SDO (term 24) -------- MOSI -----------> SI
// RC4/SDI (term 23) <------- MISO ------------ SO
// RC3/SCK (term 18) -------- SCK ------------> SCK
// RB7/CS (term 40) -------------------------> /CS
//
// Copyright, Peter H. Anderson, Baltimore, MD, Jan, '02

#case

#device PIC18C452

#include <defs_18c.h>
#include <delay.h>
#include <ser_18c.h>

#define TRUE !0
#define FALSE 0

#define B1_WRITE 0x84
#define B2_WRITE 0x87

#define B1_READ 0xd4	// see text
#define B2_READ 0xd6

#define DONT_CARE 0x00

void at45_setup_SPI(void);

void at45_buffer_write_seq_bytes(byte buffer, unsigned long adr, byte *write_dat, byte num_bytes);
void at45_buffer_read_seq_bytes(byte buffer, unsigned long adr, byte *read_dat, byte num_bytes);
void display(byte *read_dat, byte num_bytes);

void spi_io(byte *io, byte num_bytes);

void main(void)
{
    unsigned long adr;
    byte dat[16], n;

    ser_init();
	printf(ser_char, "\r\n................\r\n");

    at45_setup_SPI();

    for(adr=0x000; adr < 0x0200; adr+=16)						// write a block of eight bytes
    {
		for (n=0; n<16; n++)
		{
		   dat[n] =  (adr % 10) + n;
	    }
	    at45_buffer_write_seq_bytes(1, adr, dat, 16);
	}

    for (adr=0x000; adr < 0x0200; adr+=16)
    {
		at45_buffer_read_seq_bytes(1, adr, dat, 16);
		display(dat, 16);
	}

	while(1)
	{
	}
}

void at45_setup_SPI(void)
{
    sspen = 0;
    sspen = 1;
    sspm3 = 0; sspm2 = 0; sspm1 = 1; sspm0 = 0;	// Configure as SPI Master, fosc / 64
    ckp = 0; 							// idle state for clock is zero
    stat_cke = 1;						// data transmitted on rising edge
    stat_smp = 1; 						// input data sampled at end of clock pulse

    latc3 = 0;
    trisc3 = 0;	// SCK as output 0

    trisc4 = 1;	// SDI as input
    trisc5 = 0;	// SDO as output

    latb7 = 1;		// CS for 25LC640
	trisb7 = 0;
}


void at45_buffer_write_seq_bytes(byte buffer, unsigned long adr, byte *write_dat, byte num_bytes)
{
    byte io[20], n;

	if (buffer == 1)
	{
	    io[0] = B1_WRITE;
	}

	else
	{
		io[0] = B2_WRITE;
	}
	io[1] = DONT_CARE;

    io[2] = (byte) (adr >> 8);
    io[3] = (byte) adr;

    for (n=0; n<num_bytes; n++)
    {
 	   io[n+4] = write_dat[n];
    }

    latb7 = 0;
	spi_io(io, num_bytes + 4);
	latb7 = 1;
}

void at45_buffer_read_seq_bytes(byte buffer, long adr, byte *read_dat, byte num_bytes)
{
    byte io[21], n;

 	if (buffer == 1)
	{
	    io[0] = B1_READ;
	}
   else
   {
       io[0] = B2_READ;
   }
	io[1] = DONT_CARE;

    io[2] = (byte) (adr >> 8);
	io[3] = (byte) adr;
    io[4] = DONT_CARE;

	latb7 = 0;		// CS low - begin sequence
    spi_io(io, num_bytes + 5);
    latb7 = 1;

    for (n=0; n<num_bytes; n++)
    {
		read_dat[n] = ~io[n+5];
	}
}

void display(byte *read_dat, byte num_bytes)
{
    byte n;

    printf(ser_char, "\n\r");

	for (n=0; n<num_bytes; n++)
	{
	   ser_hex_byte(read_dat[n]);
	   ser_char(' ');

	   if (((n+1)%16) == 0)		// 16 values per line
	   {
		   printf(ser_char, "\n\r");
	   }
	}
}

void spi_io(byte *io, byte num_bytes)
{
	byte n;

	for(n=0; n<num_bytes; n++)
	{

        SSPBUF = io[n];
        while(!stat_bf)   /* loop */         ;
        io[n] = SSPBUF;
	}
}

#include <delay.c>
#include <ser_18c.c>




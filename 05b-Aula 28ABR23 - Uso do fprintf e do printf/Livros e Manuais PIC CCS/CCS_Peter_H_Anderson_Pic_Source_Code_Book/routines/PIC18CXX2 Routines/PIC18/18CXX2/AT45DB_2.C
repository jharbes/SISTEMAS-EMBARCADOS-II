// AT45DB_2.C
//
// Illustrates the use of the SSP module in interfacing with an AT45DB311
// EEPROM using the SPI protocol.
//
// Writes 512 bytes data, 16 bytes at a time to Buffer 1 and then writes the buffer
// to EEPROM page 0x0000.  While the programming is being performed, the program
// writes 512 bytes to Buffer 2, 16 bytes at a time and then writes buffer 2 to EEPROM
// page 0x0001.  Continues to Buffer 1, etc.
//
// The program then reads directly from program memory and displays to the terminal.
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

#define B1MEM_WT 0x83
#define B2MEM_WT 0x86
#define MEM_RD 0xd2

#define DONT_CARE 0x00

void at45_setup_SPI(void);

void at45_buffer_to_memory(byte buffer, unsigned long page);
void at45_buffer_write_seq_bytes(byte buffer, unsigned long adr, byte *write_dat, byte num_bytes);
void at45_memory_read_seq_bytes(unsigned  long page, unsigned long adr, byte *read_dat, byte num_bytes);

void spi_io(byte *io, byte num_bytes);
void fetch_data_16bytes(byte *dat);

void main(void)
{
    unsigned long page, adr;
    byte dat[16], n, buffer;

    ser_init();
	printf(ser_char, "\r\n................\r\n");

    at45_setup_SPI();

    for (page = 0x0001; page < 0x0003; page++)	// 3 * 512 bytes
    {
		if ((page%2) == 0)	// if even page, use Buffer 1
		{
			buffer = 1;
		}
		else
		{
			buffer = 2;		// for odd pages, use Buffer 2
		}

		for (adr = 0x000; adr < 0x200; adr+=16)
		{
			fetch_data_16bytes(dat);
			at45_buffer_write_seq_bytes(buffer, adr, dat, 16);   // write to buffer
		}
		at45_buffer_to_memory(buffer, page);	// buffer programmed to EEPROM
      delay_ms(1);
	}

   delay_ms(25);     // allow for final page to be programmed

    for (page = 0x0001; page < 0x0003; page++)	// now, display
    {
		for (adr = 0x000; adr < 0x0200; adr+=16)
		{
		   at45_memory_read_seq_bytes(page, adr, dat, 16);
		   printf(ser_char, "%4lx:%4lx: ", page, adr);
		   for (n=0; n<16; n++)
		   {
			   printf(ser_char, "%2x ", dat[n]);
		   }
		   printf(ser_char, "\r\n");
	    }
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

void at45_buffer_to_memory(byte buffer, unsigned long page)
{
	byte io[4];
	unsigned long x;

	if (buffer == 1)
	{
		io[0] = B1MEM_WT;
	}
	else
	{
		io[0] = B2MEM_WT;
	}

	x = page << 2;	// 0 Pa12, Pa11
	io[1] = (byte) (x>>8);	// high 7 bits
	io[2] = (byte) (x);
	io[3] = DONT_CARE;
   	latb7 = 0;
	spi_io(io, 4);
	latb7 = 1;
}

void at45_memory_read_seq_bytes(unsigned  long page, unsigned long adr, byte *read_dat, byte num_bytes)
{
	byte io[25], n;
	unsigned long x;

	io[0] = MEM_RD;	// read from program memory
	x = page << 2;	// 0 Pa12, Pa11
	io[1] = (byte) (x>>8);	// high 7 bits
	io[2] = ((byte) (x)) + ((byte) (adr >> 8) & 0x03);
	io[3] = (byte) (adr);
	io[4] = DONT_CARE;
	io[5] = DONT_CARE;
	io[6] = DONT_CARE;
	io[7] = DONT_CARE;

	latb7 = 0;
	spi_io(io, num_bytes + 8);
	latb7 = 1;

	for (n=0; n<num_bytes; n++)
	{
	   read_dat[n] = ~io[n+8];
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

void fetch_data_16bytes(byte *dat)	// this is a stub that generates 16 values
{
	static byte m = 0x00;
	byte n;

	for (n=0; n<16; n++, m++)
	{
		dat[n] = m % 23;
	}
}

#include <delay.c>
#include <ser_18c.c>




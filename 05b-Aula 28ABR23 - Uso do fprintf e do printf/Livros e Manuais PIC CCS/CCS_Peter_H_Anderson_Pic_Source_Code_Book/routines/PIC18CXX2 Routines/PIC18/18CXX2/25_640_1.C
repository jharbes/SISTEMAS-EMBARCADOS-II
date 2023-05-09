// 25_640_1.C
//
// Illustrates the use of the SSP module in interfacing with a 25LC640
// EEPROM using the SPI protocol.  Illustrates single byte mode.
//
// Uses single byte write and read.  Writes 10 values beginning at location
// 0x0700 and then reads back the values and displays them on the terminal.
//
// Configures bit BP1 and BP0 in the status register of the 25LC640 for write
// protection of the entire meory.  Writes new values and then reads.  Note
// that the new values are not programmed to the EEPROM
//
//	PIC18F452								25LC640
//
// RC5/SDO (term 24) -------------------> SI (term 5)
// RC4/SDI (term 23) <------------------- SO (term 2)
// RC3/SCK (term 18) -------------------> SCK (term 6)
// RB7/CS (term 40) --------------------> /CS (term 1)
//
//
// Copyright, Peter H. Anderson, Baltimore, MD, Jan, '02

#case

#device PIC18C452

#include <defs_18c.h>
#include <delay.h>
#include <ser_18c.h>

#define TRUE !0
#define FALSE 0

#define SPI_RDSR 0x05		// various 24LC640 command codes defined
#define SPI_WRSR 0x01
#define SPI_WREN 0x06
#define SPI_WRITE 0x02
#define SPI_READ 0x03

void _25_640_setup_SPI(void);

void _25_640_write_status_reg(byte dat);
byte _25_640_read_status_reg(void);

void _25_640_write_byte(unsigned long adr, byte dat);
byte _25_640_read_byte(unsigned long adr);

void spi_io(byte *io, byte num_bytes);

void main(void)
{
   byte n, dat;
   unsigned long adr;

   ser_init();
   printf(ser_char, "\r\n................\r\n");

   _25_640_setup_SPI();

   _25_640_write_status_reg(0x00);	// no block protection
   dat = _25_640_read_status_reg();
   printf(ser_char, "SR = %2x\r\n", dat);	// display status register

   for (n=0, adr = 0x0700; n<10; n++, adr++)
   {
     _25_640_write_byte(adr, n+10);		// write 10, 11, 12, etc
     ser_char('!');						// to see that something is happening
   }

   for (n=0, adr = 0x0700; n<10; n++, adr++)
   {
	  dat = _25_640_read_byte(adr);		// now read back the data
      ser_hex_byte(dat);       			// and display
      ser_char(' ');
      delay_ms(250);
   }

   printf(ser_char, "\r\n");

   _25_640_write_status_reg(0x0c);	// no block protection
   dat = _25_640_read_status_reg();
   printf(ser_char, "SR = %2x\r\n", dat);	// display status register

   for (n=0, adr = 0x0700; n<10; n++, adr++)
   {
     _25_640_write_byte(adr, n+20);		// write 20, 21, 22, etc
     ser_char('!');						// to see that something is happening
   }

   for (n=0, adr = 0x0700; n<10; n++, adr++)
   {
	  dat = _25_640_read_byte(adr);		// now read back the data
      ser_hex_byte(dat);       			// and display
      ser_char(' ');
      delay_ms(250);
   }

   while(1)
   {
   }   // continual loop
}

void _25_640_setup_SPI(void)
{
    sspen = 0;
    sspen = 1;
    sspm3 = 0; sspm2 = 0; sspm1 = 1; sspm0 = 0;	// Configure as SPI Master, fosc / 64
    ckp = 0; 							// idle state for clock is zero
    stat_cke = 1;						// data transmitted on rising edge
    stat_smp = 0; 						// input data sampled at end of clock pulse

    latc3 = 0;
    trisc3 = 0;	// SCK as output 0

    trisc4 = 1;	// SDI as input
    trisc5 = 0;	// SDO as output

    latb7 = 1;		// CS for 25LC640
    trisb7 = 0;
}

void _25_640_write_status_reg(byte dat)
{
    byte io[2];

    io[0] = SPI_WREN;

    latb7 = 0;					// CS low
    spi_io(io, 1);
    latb7 = 1;

    io[0] = SPI_WRSR;
    io[1] = dat;

    latb7 = 0;					// CS low
	spi_io(io, 2);
    latb7 = 1;

    delay_ms(5); 	// allow time for programming non-vol bits BP1 and BP0
}

byte _25_640_read_status_reg(void)
{
    byte io[2];

    io[0] = SPI_RDSR;

	latb7 = 0;
	spi_io(io, 2);
    latb7 = 1;

    return(io[1]);
}

void _25_640_write_byte(unsigned long adr, byte dat)
{
    byte io[4];

    io[0] = SPI_WREN;

    latb7 = 0;	// CS low
    spi_io(io, 1);
    latb7 = 1;					// CS high - end of WREN sequence

	io[0] = SPI_WRITE;
	io[1] = (byte) (adr >> 8);
	io[2] = (byte) adr;
	io[3] = dat;

	latb7 = 0;					// begin another session - 4 bytes
	spi_io(io, 4);				// output the 4 bytes
	latb7 = 1;

   delay_ms(25); 	// allow time for programming EEPROM
}

byte _25_640_read_byte(unsigned long adr)
{
    byte io[4];

	io[0] = SPI_READ;
	io[1] = (byte) (adr >> 8);
	io[2] = (byte) adr;

	latb7 = 0;		// CS low
    spi_io(io, 4);
    latb7 = 1;

    return(io[3]);
}

void spi_io(byte *io, byte num_bytes)
{
	byte n;

	for(n=0; n<num_bytes; n++)
	{
//        printf(ser_char, "%d...%2x\n\r", n, io[n]);

        SSPBUF = io[n];
        while(!stat_bf)   /* loop */         ;
        io[n] = SSPBUF;
	}
//   delay_ms(500);
}

#include <delay.c>
#include <ser_18c.c>

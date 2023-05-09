// 25_640_2.C
//
// Illustrates the use of the SSP module in interfacing with a 25LC640
// EEPROM using the SPI protocol.  Illustrates block write and read mode.
//
// Writes one block of eight bytes beginning to locations beginning at
// 0x0700 and another block to 0x0708.  Then reads back the values and
// displays them on the terminal.
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

void _25_640_write_seq_bytes(unsigned long adr, byte *write_dat, byte num_bytes);
void _25_640_read_seq_bytes(unsigned long adr, byte *read_dat, byte num_bytes);
void display(byte *read_dat, byte num_bytes);

void spi_io(byte *io, byte num_bytes);

void main(void)
{
	byte dat_array[8], dat, n;
	byte const a[8] = {0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00};
	byte const b[8] = {0x17, 0x16, 0x15, 0x14, 0x13, 0x12, 0x11, 0x10};

    ser_init();
	printf(ser_char, "\r\n................\r\n");

    _25_640_setup_SPI();

    _25_640_write_status_reg(0x00);	// no block protection
    dat = _25_640_read_status_reg();
    printf(ser_char, "SR = %2x\r\n", dat);	// display status register

    for(n=0; n<8; n++)						// write a block of eight bytes
    {
		dat_array[n] = a[n];
	}

    _25_640_write_seq_bytes(0x700, dat_array, 8);

    for(n=0; n<8; n++)								// write another block
    {
		dat_array[n] = b[n];
	}

	_25_640_write_seq_bytes(0x708, dat_array, 8);  // write another


    _25_640_read_seq_bytes(0x700, dat_array, 8);  // read each block and display
	display(dat_array, 8);

	_25_640_read_seq_bytes(0x708, dat_array, 8);
	display(dat_array, 8);

	while(1)
	{
	}
}

void _25_640_setup_SPI(void)
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

void _25_640_write_seq_bytes(unsigned long adr, byte *write_dat, byte num_bytes)
{
    byte io[19], n;

	io[0] = SPI_WREN;
	latb7 = 0;		// CS low - begin WREN sequence
    spi_io(io, 1);
    latb7 = 1;

    io[0] = SPI_WRITE;
	io[1] = (byte) (adr >> 7);
	io[2] = (byte) adr;

	for (n=0; n<num_bytes; n++)
	{
		io[n+3] = write_dat[n];
	}

	latb7 = 0;
	spi_io(io, num_bytes + 3);
	latb7 = 1;

	delay_ms(30);  // allow time for programming
}

void _25_640_read_seq_bytes(long adr, byte *read_dat, byte num_bytes)
{
    byte io[19], n;

    io[0] = SPI_READ;
    io[1] = (byte) (adr >> 7);
	io[2] = (byte) adr;

	latb7 = 0;		// CS low - begin sequence
    spi_io(io, num_bytes + 3);
    latb7 = 1;

    for (n=0; n<num_bytes; n++)
    {
		read_dat[n] = io[n+3];
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

	   if (((n+1)%4) == 0)		// four values per line
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




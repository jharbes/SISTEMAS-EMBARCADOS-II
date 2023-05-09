// 25_640BB.C
//
// Illustrates the interface with Microchip 25LC640 EEPROM (SPI) using a bit bang
// implementation. This is useful for PICs not having an SSP interface or in
// applications where the SSP module is used for other purposes.
//
// Uses single byte write and read.  Writes 10 values beginning at location
// 0x0700 and then reads back the values and displays them on the LCD.
//
// Note that although the PIC terminals which are used are the same as the SSP module,
// any terminals may be used when using this bit bang approach.
//
//	PIC16F877								25LC640
//
// RC5/SDO (term 24) -------------------> SI (term 5)
// RC4/SDI (term 23) <------------------- SO (term 2)
// RC3/SCK (term 18) -------------------> SCK (term 6)
// RB4/CS (term 37) --------------------> /CS (term 1)
//
// Copyright, Peter H. Anderson, Baltimore, MD, Jan, '01

#case

#device PIC16F877 *=16 ICD=TRUE

#include <defs_877.h>
#include <lcd_out.h>

#define TRUE !0
#define FALSE 0

#define SPI_WREN 0x06		// various 24LC640 command codes defined
#define SPI_WRITE 0x02
#define SPI_READ 0x03

#define SCK_DIR trisc3		// SPI terminals defined
#define SDO_DIR trisc5
#define SDI_DIR trisc4
#define CS_DIR_25640 trisb4

#define SCK_PIN portc3
#define SDO_PIN portc5
#define SDI_PIN portc4
#define CS_PIN_25640 rb4

void _25_640_setup_SPI(void);
void _25_640_write_byte(unsigned long adr, byte dat);
byte _25_640_read_byte(unsigned long adr);
byte spi_io(byte spi_byte);

void main(void)
{
   byte n, dat;
   unsigned long  adr;

   lcd_init();

   CS_PIN_25640 = 1;		// Chip Select at logic one
   CS_DIR_25640 = 0;		// output

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
    SDI_DIR = 1;		// configure SDI as input, SDO and SCK as outputs
    SDO_DIR = 0;
    SCK_DIR = 0;

    SCK_PIN = 0;		// be sure clock is at zero
}

void _25_640_write_byte(unsigned long adr, byte dat)
{
    byte dummy;

    CS_PIN_25640 = 0;					// CS low
    dummy = spi_io(SPI_WREN);
    CS_PIN_25640 = 1;				    // CS high - end of WREN sequence

	CS_PIN_25640 = 0;					// begin another session - 4 bytes
	dummy = spi_io(SPI_WRITE);
    dummy = spi_io((byte) (adr >> 8));	// high byte of adr
	dummy = spi_io((byte) adr);			// low byte of address
	dummy = spi_io(dat);				// data
	CS_PIN_25640 = 1;

    delay_ms(5); 	// allow time for programming EEPROM
}

byte _25_640_read_byte(unsigned long adr)
{
    byte dummy, dat;

    CS_PIN_25640 = 0;					// begin session - 4 bytes
	dummy = spi_io(SPI_READ);
    dummy = spi_io((byte) (adr >> 8));	// high byte of adr
	dummy = spi_io((byte) adr);			// low byte of address
	dat = spi_io(dummy);				// data
	CS_PIN_25640 = 1;

	return(dat);
}

byte spi_io(byte spi_byte)
{
    byte n;

    for(n=0; n<8; n++)
    {
       if (spi_byte & 0x80)  /* most sign bit first */
       {
		   SDO_PIN = 1;
       }
       else
       {
           SDO_PIN = 0;
       }
       SCK_PIN = 1;

       spi_byte = (spi_byte << 1) | SDI_PIN;
       SCK_PIN = 0;
   }
   return(spi_byte);
}

#include <lcd_out.c>

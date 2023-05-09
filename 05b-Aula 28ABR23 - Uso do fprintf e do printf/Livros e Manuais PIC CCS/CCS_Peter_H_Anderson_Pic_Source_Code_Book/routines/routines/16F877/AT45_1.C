// Program AT45_1.C
//
// Illustrates an interface with an Ateml AT45D011 512 page X 264 byte Flash EEPROM
// using MSSP in SPI Master Mode.
//
// Illustrates how to write (and read) a single byte and sequential bytes to (and from)
// the 264 byte RAM buffer.  Transfer of the RAM buffer to Flash EEPROM and Flash EEPROM
// to RAM buffer and direct sequential read from Flash EEPROM.
//
//	PIC16F877								AT45D011
//
// RC5/SDO (term 24) -------------------> SI (term 1)
// RC4/SDI (term 23) <------------------- SO (term 8)
// RC3/SCK (term 18) -------------------> SCK (term 2)
// RD0/CS (term 19) --------------------> CE (term 4)
//
// In this example;
//
//	/WP (term 5) and /RESET (term 4) are open (logic one).  The AT45 device includes
// internal pullup resistors on these inputs.
//
// Copyright, Peter H. Anderson, Baltimore, MD, Feb, '01

#case

#device PIC16F877 *=16 ICD=TRUE

#include <defs_877.h>
#include <lcd_out.h>

#define TRUE !0
#define FALSE 0

//ATMEL SerialDataFlash Commands

#define MEM_PAGE_READ 0x52
#define BUFF_READ 0x54
#define BUFF_WRITE 0x84
#define MEM_PAGE_TO_BUFF_TRANSFER 0x53
#define MEM_PAGE_TO_BUFF_COMPARE 0x60
#define BUFF_TO_MEM_PAGE_WITH_ERASE 0x83
#define BUFF_TO_MEM_PAGE_NO_ERASE 0x88
#define MEM_PAGE_ERASE 0x81
#define MEM_BLOCK_ERASE 0x50
#define STATUS_REG 0x57

void AT45_setup_SPI(void);

byte AT45_read_data_flash_status(void);

void AT45_write_buffer_byte(unsigned long adr, byte dat);
byte AT45_read_buffer_byte(unsigned long  adr);

void AT45_write_buffer_sequential(unsigned long adr, byte *d, byte num_bytes);
void AT45_read_buffer_sequential(unsigned long  adr, byte *d, byte num_bytes);

void AT45_buffer_to_flash_copy_with_erase(unsigned long page);
void AT45_flash_to_buffer(unsigned long page);

void AT45_read_flash_sequential(unsigned long page, unsigned long adr, byte *d, byte num_bytes);

void main(void)
{
	byte dat, n;
	byte d[4] = {0xaa, 0xbb, 0xcc, 0xdd};

    lcd_init();
    AT45_setup_SPI();
    pspmode = 0;

    dat = AT45_read_data_flash_status();	// read STATUS
    printf(lcd_char, "Status = %2x", dat);
    delay_ms(2000);

    lcd_init();								// write and read bytes, byte by byte
	printf(lcd_char, "RAM Byte Test");
	delay_ms(1000);

	for (n = 0; n<4; n++)
	{
		dat = 0xaa + n;
		AT45_write_buffer_byte(0x0000+n, dat);
	}

    lcd_clr_line(1);
	for (n = 0; n<4; n++)
	{
		dat = AT45_read_buffer_byte(0x0000+n);
		lcd_hex_byte(dat);
		lcd_char(' ');
	}

	delay_ms(2000);
	lcd_init();

    printf(lcd_char, "RAM Seq Test");		// sequential write and read
    delay_ms(1000);

    AT45_write_buffer_sequential(0x0000, d, 4);

    for (n=0; n<4; n++)
    {
		d[n] = 0;
	}

	AT45_read_buffer_sequential(0x0000, d, 4);
	lcd_clr_line(1);
	for (n = 0; n<4; n++)
	{
		lcd_hex_byte(d[n]);
		lcd_char(' ');
	}

	delay_ms(2000);

	lcd_init();

	printf(lcd_char, "Transfer Test");		// illustrates transfer of RAM to flash
											// and falsh page to RAM buffer
	for (n=0; n<4; n++)
	{
		d[n] = 0xb0 + n;
	}
	AT45_write_buffer_sequential(0x0000, d, 4);
	AT45_buffer_to_flash_copy_with_erase(0);	// transfer to page 0

	for (n=0; n<4; n++)
	{
		d[n] = 0xc0 + n;
	}

	AT45_write_buffer_sequential(0x0000, d, 4);
	AT45_buffer_to_flash_copy_with_erase(511);	// transfer to page 511

	AT45_flash_to_buffer(0);					// page 0 to buffer
	AT45_read_buffer_sequential(0x0000, d, 4);	// read and display
	lcd_clr_line(1);
	for (n = 0; n<4; n++)
	{
		lcd_hex_byte(d[n]);
		lcd_char(' ');
	}

	AT45_flash_to_buffer(511);					// page 511 to buffer
	AT45_read_buffer_sequential(0x0000, d, 4);	// read and display
	lcd_clr_line(2);
	for (n = 0; n<4; n++)
	{
		lcd_hex_byte(d[n]);
		lcd_char(' ');
	}

	delay_ms(2000);

   lcd_init();
   printf(lcd_char, "Read from Flash");

   AT45_read_flash_sequential(0, 0x0000, d, 4);
   lcd_clr_line(1);
 	for (n = 0; n<4; n++)
	{
		lcd_hex_byte(d[n]);
		lcd_char(' ');
	}

    AT45_read_flash_sequential(511, 0x0000, d, 4);
    lcd_clr_line(2);
 	for (n = 0; n<4; n++)
	{
		lcd_hex_byte(d[n]);
		lcd_char(' ');
	}

	delay_ms(2000);

	lcd_init();
	printf(lcd_char, "Done");

	while(1)		;
}

void AT45_setup_SPI(void)
{
    sspen = 0;
    sspen = 1;
    sspm3 = 0; sspm2 = 0; sspm1 = 1; sspm0 = 0;	// Configure as SPI Master, fosc / 64
    ckp = 1; 							// idle state for clock is zero
    stat_cke = 1;						// data transmitted on rising edge
    stat_smp = 1; 						// input data sampled at end of clock pulse

    portc3 = 1;
    trisc3 = 0;	// SCK as output 0

    trisc4 = 1;	// SDI as input
    trisc5 = 0;	// SDO as output

    portd0 = 0;
    trisd0 = 0;
}

byte AT45_read_data_flash_status(void)
{
   byte d, dummy;

   portd0 = 0;

   SSPBUF = STATUS_REG;
   while(!stat_bf)		;
   dummy = SSPBUF;

   SSPBUF = dummy;
   while(!stat_bf)		;
   d = SSPBUF;

   portd0 = 1;

   return(d);
}

void AT45_write_buffer_byte(unsigned long adr, byte dat)
{
	byte dummy;

	portd0 = 0;
    SSPBUF = BUFF_WRITE;
    while(!stat_bf)		;
    dummy = SSPBUF;

    SSPBUF = dummy;
	while(!stat_bf)		;
    dummy = SSPBUF;

    SSPBUF = adr >> 8;
	while(!stat_bf)		;
    dummy = SSPBUF;

    SSPBUF = adr & 0xff;
	while(!stat_bf)		;
	dummy = SSPBUF;

    SSPBUF = dat;
	while(!stat_bf)		;
	dummy = SSPBUF;

	portd0 = 1;
}

byte AT45_read_buffer_byte(unsigned long adr)
{
	byte dummy, dat;

	portd0 = 0;
    SSPBUF = BUFF_READ;
    while(!stat_bf)		;
    dummy = SSPBUF;

    SSPBUF = dummy;
	while(!stat_bf)		;
    dummy = SSPBUF;

    SSPBUF = adr >> 8;
	while(!stat_bf)		;
    dummy = SSPBUF;

    SSPBUF = adr & 0xff;
	while(!stat_bf)		;
	dummy = SSPBUF;

    SSPBUF = dummy;			// dummy byte
	while(!stat_bf)		;
	dummy = SSPBUF;

    SSPBUF = dummy;
	while(!stat_bf)		;
	dat = SSPBUF;

	portd0 = 1;

	return(dat);
}

void AT45_write_buffer_sequential(unsigned long adr, byte *d, byte num_bytes)
{
	byte dummy, n;

	portd0 = 0;
    SSPBUF = BUFF_WRITE;
    while(!stat_bf)		;
    dummy = SSPBUF;

    SSPBUF = dummy;
	while(!stat_bf)		;
    dummy = SSPBUF;

    SSPBUF = adr >> 8;
	while(!stat_bf)		;
    dummy = SSPBUF;

    SSPBUF = adr & 0xff;
	while(!stat_bf)		;
	dummy = SSPBUF;

    for (n=0; n<num_bytes; n++)
    {
		SSPBUF = d[n];
	    while(!stat_bf)		;
	    dummy = SSPBUF;
	}

	portd0 = 1;

}
void AT45_read_buffer_sequential(unsigned long  adr, byte *d, byte num_bytes)
{
	byte dummy, n;

	portd0 = 0;
    SSPBUF = BUFF_READ;
    while(!stat_bf)		;
    dummy = SSPBUF;

    SSPBUF = dummy;
	while(!stat_bf)		;
    dummy = SSPBUF;

    SSPBUF = adr >> 8;
	while(!stat_bf)		;
    dummy = SSPBUF;

    SSPBUF = adr & 0xff;
	while(!stat_bf)		;
	dummy = SSPBUF;

    SSPBUF = dummy;			// dummy byte
	while(!stat_bf)		;
	dummy = SSPBUF;

	for (n=0; n<num_bytes; n++)
	{
       SSPBUF = dummy;
	   while(!stat_bf)		;
	   d[n] = SSPBUF;
    }
	portd0 = 1;
}

void AT45_buffer_to_flash_copy_with_erase(unsigned long page)
{
	byte dummy, h, l;

    page = page << 1;
	portd0 = 0;

	SSPBUF = BUFF_TO_MEM_PAGE_WITH_ERASE;
	while(!stat_bf)		;
	dummy = SSPBUF;

	h = (page >> 8) & 0x03; 	// highest two bits
	SSPBUF = h;
	while(!stat_bf)		;
	dummy = SSPBUF;

	l = page & 0xff;		// low 7 address bits
	SSPBUF = l;
	while(!stat_bf)		;
	dummy = SSPBUF;

	SSPBUF = dummy;
	while(!stat_bf)		;
	dummy = SSPBUF;

	portd0 = 1;
	delay_ms(20);				// allow time for programming
}

void AT45_flash_to_buffer(unsigned long page)
{
	byte dummy, h, l;
        page = page << 1;

	portd0 = 0;

	SSPBUF = MEM_PAGE_TO_BUFF_TRANSFER;
	while(!stat_bf)		;
	dummy = SSPBUF;

	h = (page >> 8) & 0x03; 	// highest two bits
	SSPBUF = h;
	while(!stat_bf)		;
	dummy = SSPBUF;

	l = page & 0xff;		// low 7 address bits
	SSPBUF = l;
	while(!stat_bf)		;
	dummy = SSPBUF;

	SSPBUF = dummy;
	while(!stat_bf)		;
	dummy = SSPBUF;

	portd0 = 1;
	delay_ms(1);
}

void AT45_read_flash_sequential(unsigned long page, unsigned long adr, byte *d, byte num_bytes)
{
	byte dummy, h, l, n;

	portd0 = 0;

	SSPBUF = MEM_PAGE_READ;
	while(!stat_bf)		;
	dummy = SSPBUF;

	page = page << 1;
	h = (page >> 8) & 0x03;
	l = (page & 0xff) + (adr >> 8);

	SSPBUF = h;
	while(!stat_bf)		;
	dummy = SSPBUF;

	SSPBUF = l;
	while(!stat_bf)		;
	dummy = SSPBUF;

	SSPBUF = adr & 0xff;
	while(!stat_bf)		;
	dummy = SSPBUF;

	for (n=0; n<4; n++)		// 32 don't care bts
	{
		SSPBUF = dummy;
		while(!stat_bf)		;
	    dummy = SSPBUF;
	}

	for (n=0; n<num_bytes; n++)		// 32 don't care bts
	{
		SSPBUF = dummy;
		while(!stat_bf)		;
	    d[n] = SSPBUF;
	}

	portd0 = 1;
}

#include <lcd_out.c>

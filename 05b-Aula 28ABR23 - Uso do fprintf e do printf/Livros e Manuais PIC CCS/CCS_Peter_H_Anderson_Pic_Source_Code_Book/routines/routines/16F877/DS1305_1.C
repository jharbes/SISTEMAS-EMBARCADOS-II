// DS1305_1.C
//
// Illustrates interface with DS1305 real time clock and RAM.
//
// Configures control register to zero WP bit and enable oscillator.
// Writes a time and date to timer registers (addresses 0x00 - 0x06)
// and then reads and displays time and date at nominally 2 sec
// intervals.
//
// Illustrates how to write to and read from RAM at locations 0x20 -
// 0x7f.
//
// Illustrates how to fetch the time and calculate the number of
// elapsed seconds since the start of the day.
//
//	PIC16F877  			   DS1305			To Other SPI Dev
//
// RC5/SDO (term 24) -------------------> SI (term 12) ------>
// RC4/SDI (term 23) <------------------- SO (term 13) <------
// RC3/SCK (term 18) -------------------> SCK (term 11) ----->
// RB0/CS (term 33) --------------------> CE (term 10)
//
// In this example;
//
//  VCC2 (term 1)  +5VDC
//  VCC1 (term 16) GRD
//  VBAT (term 2) GRD
//  VCCIF (term 14) +5 VDC (Determines level of logic one)
//  SERMODE (term 9) +5VDC (SPI Mode)
//  X1, X2 (terms 3, 4) 32.768 kHz Crystal
//  INT0, /INT1, /PF - Open (Not used)
//
// Copyright, Peter H. Anderson, Baltimore, MD, Feb, '01

#case

#device PIC16F877 *=16 ICD=TRUE

#include <defs_877.h>
#include <lcd_out.h>

#define TRUE !0
#define FALSE 0

#define SEC 0
#define MINI 1
#define HOUR 2
#define DAY 3
#define DATE 4
#define MONTH 5
#define YEAR 6

void _1305_setup_SPI(void);
void _1305_write_config(byte control_byte);
void _1305_write_date_time(byte *dt);
void _1305_read_date_time(byte *dt);
void _1305_write_ram(byte adr, byte *d, byte num_bytes);
void _1305_read_ram(byte adr, byte *d, byte num_bytes);

void display_date_time(byte *dt);
void display_RAM_data(byte *d, byte num_ele);

float timer(void);

byte nat_to_BCD(byte x);
byte BCD_to_nat(byte x);

void main(void)
{
    byte dt[7] = {0x00, 0x59, 0x11, 0x00, 0x28, 0x02, 0x01};
				 // Feb 28, '01  11:59:00
    byte RAM_dat[10];
    byte n;

    float elapsed_time;

    lcd_init();
    _1305_setup_SPI();
    _1305_write_config(0x00);
    _1305_write_date_time(dt);

    for (n = 0; n < 10; n++)		// continually read and display
									// date and time
			 						// this section requires nominal 20
									// secs to execute
    {
	  _1305_read_date_time(dt);
	  display_date_time(dt);
	  delay_ms(2000);
    }

    lcd_init();				// illustrate writing to and
							// reading from RAM
    printf(lcd_char, "RAM Test");
    delay_ms(1000);

    for (n = 0; n<10; n++)
    {
	  RAM_dat[n+0x20] = 0xff - n;	// fill the array with some data
    }
    _1305_write_ram(0x00 + 0x20, RAM_dat, 10);
									// write data to DS1305 RAM

    _1305_read_ram(0x00, RAM_dat, 10);	// read the data back
    display_RAM_data(RAM_dat, 10);

    delay_ms(1000);

    lcd_init();
    while(1)				// continually display elapsed time
							// since midnight
    {
	   elapsed_time = timer();
	   lcd_clr_line(0);
	   printf(lcd_char, "ET = %5.0f", elapsed_time);
	   delay_ms(1500);
    }
}

void _1305_setup_SPI(void)
{
    sspen = 0;
    sspen = 1;
    sspm3 = 0; sspm2 = 0; sspm1 = 1; sspm0 = 0;
							// Configure as SPI Master, fosc / 64
    ckp = 0; 				// idle state for clock is zero
    stat_cke = 0;
    stat_smp = 0;

    portc3 = 0;
    trisc3 = 0;				// SCK as output 0

    trisc4 = 1;				// SDI as input
    trisc5 = 0;				// SDO as output

    rb0 = 0;
    trisb0 = 0;
}

void _1305_write_config(byte control_byte)
{
	byte dummy;

	rb0 = 1;

	SSPBUF = 0x8f;
	while(!stat_bf)   /* loop */         ;
	dummy = SSPBUF;

	SSPBUF = control_byte;
	while(!stat_bf)   /* loop */         ;
	dummy = SSPBUF;

	rb0 = 0;
}

void _1305_write_date_time(byte *dt)
{
	byte dummy, n;

	rb0 = 1;

	SSPBUF = 0x80;
	while(!stat_bf)   /* loop */         ;
	dummy = SSPBUF;

	for (n=0; n<7; n++)
	{
	   SSPBUF = dt[n];
	   while(!stat_bf)		;
	   dummy = SSPBUF;
    }

    rb0 = 0;
}

void _1305_read_date_time(byte *dt)
{
	byte dummy, n;

	rb0 = 1;

	SSPBUF = 0x00;
	while(!stat_bf)   /* loop */         ;
	dummy = SSPBUF;

	for (n=0; n<7; n++)
	{
	   SSPBUF = dummy;
	   while(!stat_bf)		;
	   dt[n] = SSPBUF;
    }

    rb0 = 0;
}

void _1305_write_ram(byte adr, byte *d, byte num_bytes)
{
	byte dummy, n;

	rb0 = 1;

	SSPBUF = adr + 0x80;
	while(!stat_bf)   /* loop */         ;
	dummy = SSPBUF;

	for (n=0; n<num_bytes; n++)
	{
	   SSPBUF = d[n];
	   while(!stat_bf)		;
	   dummy = SSPBUF;
	}

	rb0 = 0;
}

void _1305_read_ram(byte adr, byte *d, byte num_bytes)
{
	byte dummy, n;

	rb0 = 1;

	SSPBUF = adr;
	while(!stat_bf)   /* loop */         ;
	dummy = SSPBUF;

	for (n=0; n<num_bytes; n++)
	{
	   SSPBUF = dummy;
	   while(!stat_bf)		;
	   d[n] = SSPBUF;
	}

	rb0 = 0;
}

void display_date_time(byte *dt)
{
   static byte line = 0;

   lcd_clr_line(line);

   lcd_hex_byte(dt[MONTH]);
   lcd_char('/');
   lcd_hex_byte(dt[DATE]);
   lcd_char('/');
   lcd_hex_byte(dt[YEAR]);

   lcd_char(' ');

   lcd_hex_byte(dt[HOUR]);
   lcd_char(':');
   lcd_hex_byte(dt[MINI]);
   lcd_char(':');
   lcd_hex_byte(dt[SEC]);

   ++line;
   if (line == 4)
   {
      line = 0;
   }
}

void display_RAM_data(byte *d, byte num_ele)
{
	byte line = 0, m, n;
	lcd_clr_line(line);
	for (n = 0, m = 0; n<num_ele; n++, m++)
	{
	    if (m==4)
	    {
		  m=0;
		  ++line;
		  if (line == 4)
		  {
			line = 0;
		  }
		  lcd_clr_line(line);
	    }

	    lcd_hex_byte(d[n]);
	    lcd_char(' ');
	}
}

float timer(void)
{
	float elapsed_time;
	byte dt[8];
	_1305_read_date_time(dt);
	elapsed_time = 3600.0 * (float) BCD_to_nat(dt[HOUR])
	         + 60.0 * (float) BCD_to_nat(dt[MINI])
	         + (float) BCD_to_nat(dt[SEC]);

	return(elapsed_time);
}

byte nat_to_BCD(byte x)
{
	byte h_nib, l_nib;
	h_nib = x/10;
	l_nib = x % 10;
	return ((h_nib << 4) | l_nib);
}

byte BCD_to_nat(byte x)
{
	byte h_nib, l_nib;
	h_nib = x >> 4;
	l_nib = x & 0x0f;
	return(10 * h_nib + l_nib);
}

#include <lcd_out.c>

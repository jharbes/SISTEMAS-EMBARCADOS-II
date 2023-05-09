// AT45DB_3.C
//
// This is an example of a data logger using the AT45DB321.
//
// On boot, the program reads input PORTB7 and if at one, perfroms
// A/D conversions (1000/sec) and saves to the AT45 EEPROM.
//
// If, on boot, PORTB7 is at zero, the content of the AT45 EEPROM is
// read and displayed to the terminal.
//
// Uses Timer 3 in conjunction with CCP2 in the "trigger special event" mode
// to reset the timer every 1000 us and also perform an A/D conversion on AN0.
// Note that only the high byte of the A/D conversion is saved to EEPROM
//
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

byte ad_buff[16], ad_buff_index;	// global variables

void main(void)
{
    unsigned long page, adr;
    byte dat[16], n, buffer;

    ser_init();
	printf(ser_char, "\r\n................\r\n");

    at45_setup_SPI();

    not_rbpu = 0;

    if (!portb7)		// is at zero, dump the data to the terminal
    {
		for (page = 0x0000; page < 0x0003; page++)
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

    else
    {

		// configure A/D
		pcfg3 = 0; pcfg2 = 1; pcfg1 = 0; pcfg0 = 0; 	// config A/D for 3/0

		adfm = 0;   // left justified
		adcs2 = 0; adcs1 = 1; adcs0 = 1; // internal RC

		adon=1;  // turn on the A/D
		chs2=0;  chs1=0;   chs0=0;

		// config timer 3
		t3rd16 = 1;
		t3ckps1 = 0;	t3ckps0 = 0;	// 1:1 prescale
		tmr3cs = 0;	// internal clock - 1 usec per tick
		TMR3H = 0x00; TMR3L = 0x00;

		// assign timers
		t3ccp2 = 0;		t3ccp1 = 1;	// assign timer3 to CCP2, timer1 to CCP1


		// config ccp2
		ccp2m3 = 1;  ccp2m2 = 0; ccp2m1 = 1;  ccp2m0 = 1;	// special event - resets Timer 3 and initiates A/D
		CCPR2H = (byte) (1000 >> 8);
		CCPR2L = (byte) (1000);

		 // turn on timers and config interrupts
		tmr3on = 1;

		ccp2if = 0;
		ccp2ie = 1;
		peie = 1;
		gieh = 1;

		ad_buff_index = 0;

        for (page = 0x0000; page < 0x0003; page++)	// 256 * 512 bytes
        {
 		   if ((page%2) == 0)
		   {
		 	   buffer = 1;
		   }
		   else
		   {
			   buffer = 2;
		   }

		   for (adr = 0x000; adr < 0x200; adr+=16)
		   {
			   fetch_data_16bytes(dat);
			   at45_buffer_write_seq_bytes(buffer, adr, dat, 16);
		   }
		   at45_buffer_to_memory(buffer, page);
	    }

	    while(gieh)	// clean up
	    {
			gieh = 0;
		}

		tmr3on = 0;	// turn off the timer
		ccp2ie = 0;
		ccp2if = 0;

      printf(ser_char, "\r\nDone!!!!!!!!!!!\r\n");
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

	io[0] = MEM_RD;
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

void fetch_data_16bytes(byte *dat)
{
	byte n;
	while (ad_buff_index < 16)	// wait for 16 samples
	{
    }

    ad_buff_index = 0;

	for (n=0; n<16; n++)
	{
		dat[n] = ad_buff[n];
	}
}

#int_ccp2
ccp2_int_handler(void)
{
#ifdef TRIAL
	ad_buff[ad_buff_index] = 16 - ad_buff_index;	// for testing
#else
	ad_buff[ad_buff_index] = 16 - ADRESH;
#endif
	++ad_buff_index;
}

#int_default
default_int_handler(void)
{
}

#include <delay.c>
#include <ser_18c.c>




// SPI_SLV2.C (PIC16F877)
//
// Illustrates an implementation of a PIC in an SPI slave application.
//
// Program loops, awaiting SSP interrupt when input SS is low and a byte has been
// received from the master.  Sends the nine byte result of the previous measurement.
//
// Processor then performs a temperature measurement on the specified DS1820.
//
//	BX24							 PIC16F877
//
// MOSI (term 15) -----------------> RC4/SDI (term 23
// MISO (term 16) <----------------- RC5/SDO (term 24)
// SCK (term 17) ------------------> RC3/SCK (term 18)
// CS (term 18) -------------------> RA5/AN4/SS (term 7)
//
// copyright, Peter H. Anderson, Georgetown, SC, Mar, '01

#case

#device PIC16F877 *=16 ICD=TRUE

#include <defs_877.h>
#include <lcd_out.h>
#include <_1_wire.h>

#define TRUE !0
#define FALSE 0

void SPI_setup_slave(void);
void DS1820_make_temperature_meas(byte sensor, byte *buff);

byte ssp_int_occurred;

void main(void)
{
   byte buff[9];
   byte n, command, channel, dummy;

   lcd_init();

   SPI_setup_slave();

   while(1)
   {
	   sspif = 0;	// kill any pending interrupt
	   peie = 1;
	   sspie = 1;
	   ssp_int_occurred = FALSE;
	   gie = 1;

	   SSPBUF = 0x00;
	   while(!ssp_int_occurred)	/* loop */	;
	   command = SSPBUF;

       ssp_int_occurred = FALSE;

  	   for (n=0; n<9; n++)
	   {
	      SSPBUF = buff[n];
	      while(!ssp_int_occurred)	/* loop */ ;
	      dummy = SSPBUF;
		  ssp_int_occurred = FALSE;
	   }

	   while(gie)
	   {
	      gie = 0;
	   }

	   channel = command - 0x80;
	   DS1820_make_temperature_meas(channel, buff);	// perform a new temperature measurement
   }
}

void SPI_setup_slave(void)
{
	sspen = 0;

	pcfg3 = 0;  pcfg2 = 1;  pcfg1 = 0;  pcfg0 = 0;	// config A/Ds as 3/0

    sspm3 = 0;  sspm2 = 1;  sspm1 = 0;  sspm0 = 0;	// Configure as SPI Slave, /SS enabled
    ckp = 0; // idle state for clock is zero

    stat_cke = 0;  		 // data transmitted on falling edge
    stat_smp = 0;		 // for slave mode

    trisc3 = 1;	// SCK as input
    trisc4 = 1;	// SDI as input
    trisc5 = 0;	// SDO as output
    trisa5 = 1; // SS an input

    sspen = 1;
}


void DS1820_make_temperature_meas(byte sensor, byte *buff)
{
   byte n;

   pspmode = 0;	// configure parallel slave port as general purpose port

   _1w_init(sensor);
   _1w_out_byte(sensor, 0xcc);  // skip ROM

   _1w_out_byte(sensor, 0x44);  // perform temperature conversion
   _1w_strong_pull_up(sensor);

   _1w_init(sensor);
   _1w_out_byte(sensor, 0xcc);  // skip ROM

   _1w_out_byte(sensor, 0xbe);

   for (n=0; n<9; n++)
   {
      buff[n] = _1w_in_byte(sensor);
   }
}

#int_ssp ssp_int_handler(void)
{
	ssp_int_occurred = TRUE;
}

#int_default default_interrupt_handler(void)
{
}

#include <lcd_out.c>
#include <_1_wire.c>

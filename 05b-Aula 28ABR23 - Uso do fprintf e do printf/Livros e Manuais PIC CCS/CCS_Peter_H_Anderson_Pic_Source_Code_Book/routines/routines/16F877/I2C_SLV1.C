// I2C_SLV1.C
//
// Illustrates use of a PIC16F87X as an I2C Slave device.  The slave address
// is fixed at 0x40 (SSPADD = 0x40).
//
// The program loops waiting for an interrupt which occurs when the assigned slave
// address is received.  If the I2C address byte is a read,  nine bytes are sent to
// the master.  Otherwise, the PIC loops, waiting for a command and on receipt of the
// command. performs a task.
//
// BX24					Serial LCD+				PIC16F877
//
// Term 13 -------------> (term 2)
//
// Term 15 <------------------------------------> RC4/SDA (term 23)
// Term 14 -------------------------------------> RC3/SCL (term 18)
//
// copyright, Peter H. Anderson, Georgetown, SC, Mar, '01

#case

#device PIC16F877 *=16 ICD=TRUE

#include <defs_877.h>
#include <lcd_out.h>

#define TRUE !0
#define FALSE 0

void I2C_slave_setup(void);
void I2C_slave_send_bytes(byte *buff, byte num_bytes);
void do_task(byte *buff);

byte ssp_int_occurred;

void main(void)
{
    byte dummy, command;
    byte buff[9] = {0xf0, 1, 2, 3, 4, 5, 6, 7, 8};

    I2C_slave_setup();
	ssp_int_occurred = FALSE;
	gie = 1;
	while(1)
	{
	   while(!ssp_int_occurred)			/* loop */   ;

	   ssp_int_occurred = 0;
	   dummy = SSPBUF;

	   if(stat_rw)				// it was a read command
	   {
	       I2C_slave_send_bytes(buff, 9);
	   }

	   else
	   {
		   while(!ssp_int_occurred)	/* loop waiting for command*/	;

		   ssp_int_occurred = 0;
		   command = SSPBUF;
		   do_task(buff);
	   }
   }
}

void do_task(byte *buff)
{
    byte n;
	for (n=0; n<9; n++)
	{
	   ++buff[n];
	}
}


void I2C_slave_send_bytes(byte *buff, byte num_bytes)
{
   byte n;
   for (n=0; n<num_bytes; n++)
   {
	   SSPBUF = buff[n];
       ckp = 1;
       n=n;  // debugging
	   while(!ssp_int_occurred)	/* loop waiting for ack */	;
	   ssp_int_occurred = FALSE;
	}
}

void I2C_slave_setup(void)
{
// set that GEI is not set in this routine

   sspen = 0;

   sspm3 = 0;  sspm2 = 1;  sspm1 = 1; sspm0 = 0;	// I2C Slave Mode - 7-bit

   trisc3 = 1;	// SCL an input
   trisc4 = 1;  // SDA an input

   gcen = 0;	// no general call
   stat_smp = 0; 	// slew rate controlled
   stat_cke = 0;	// for I2C
   ckp = 1;		// no stretch

   SSPADD = 0x480;

   sspif = 0;
   sspie = 1;
   peie = 1;

   sspen = 1;	// enable the module
}

#int_ssp ssp_int_handler(void)
{
	ssp_int_occurred = TRUE;
}

#int_default default_interrupt_handler(void)
{
}

#include <lcd_out.c>

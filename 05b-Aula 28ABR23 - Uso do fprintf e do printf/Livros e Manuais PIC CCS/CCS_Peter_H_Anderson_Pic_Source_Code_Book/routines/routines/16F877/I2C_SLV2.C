// I2C_SLV2.C
//
// Illustrates use of a PIC16F87X as an I2C Slave device.  The slave address
// is fixed at 0x40 (SSPADD = 0x40).
//
// The program loops waiting for an interrupt which occurs when the assigned slave
// address is received.  If the I2C address byte is a read,  nine bytes are sent to
// the master.  Otherwise, the PIC loops, waiting for a command and on receipt of the
// command, performs a task.
//
// If the command is 0x8X, the slave performs a temperature measurement using a Dallas
// DS1820 on the specified channel.  If the command is 0x7X, the slave flashes an LED X
// times.
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
#include <_1_wire.h>

#define TRUE !0
#define FALSE 0

void I2C_slave_setup(void);
void I2C_slave_send_bytes(byte *buff, byte num_bytes);

void flash_led(byte num_flashes);
void DS1820_make_temperature_meas(byte sensor, byte *buff);

byte ssp_int_occurred;

void main(void)
{
    byte dummy, command, buff[9];

    pspmode = 0;

    I2C_slave_setup();
    ssp_int_occurred = FALSE;
    gie = 1;

	while(1)
	{
	   while(!ssp_int_occurred)			/* loop */   ;

	   ssp_int_occurred = FALSE;
	   dummy = SSPBUF;

	   if(stat_rw)				// it was a read command
	   {
	       I2C_slave_send_bytes(buff, 9);
	   }

	   else
	   {
		   while(!ssp_int_occurred)	/* loop waiting for command*/	;

	 	   ssp_int_occurred = FALSE;
		   command = SSPBUF;
		   if((command >> 4) == 7)	// if high nibble is a 7
		   {
			   flash_led(command & 0x0f);
			   // flash number of times in low nibble of command
		   }
		   else if ((command >> 4) == 8)	// high nibble is an 8
		   {
			   // perform measurement on channel in lower nibble
			   DS1820_make_temperature_meas(command&0x0f, buff);
		   }
		}
   }
}

void flash_led(byte num_flashes)
{
   byte n;
   portd7 = 0;
   trisd7 = 0;	// make LED pin an output

   for (n=0; n<num_flashes; n++)
   {
	    portd7 = 1;
		delay_ms(40);
		portd7 = 0;
		delay_ms(40);
   }
}

void DS1820_make_temperature_meas(byte sensor, byte *buff)
{
   byte n;

   _1w_init(sensor);
   _1w_out_byte(sensor, 0xcc);  // skip ROM

   _1w_out_byte(sensor, 0x44);  // perform temperature conversion
   _1w_strong_pull_up(sensor);

   _1w_init(sensor);
   _1w_out_byte(sensor, 0xcc);  // skip ROM

   _1w_out_byte(sensor, 0xbe);

   for (n=0; n<9; n++)
   {
      buff[n]=_1w_in_byte(sensor);
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
// note that GEI is not set in this routine

   sspen = 0;

   sspm3 = 0;  sspm2 = 1;  sspm1 = 1; sspm0 = 0;	// I2C Slave Mode - 7-bit

   trisc3 = 1;	// SCL an input
   trisc4 = 1;  // SDA an input

   gcen = 0;	// no general call
   stat_smp = 0; 	// slew rate controlled
   stat_cke = 0;	// for I2C
   ckp = 1;		// no stretch

   SSPADD = 0x40;

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
#include <_1_wire.c>

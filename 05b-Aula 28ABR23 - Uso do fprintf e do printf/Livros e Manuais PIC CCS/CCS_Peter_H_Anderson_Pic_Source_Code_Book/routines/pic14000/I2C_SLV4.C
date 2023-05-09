// I2C_SLV4.C PIC14000
//
// SCL/RC6, SDA/RC7
//
// copyright, Peter H. Anderson, Montpelier, VT, June, '02

#case

#device PIC14000 *=16

#include <defs_14.h>
#include <delay.h>
#include <ser_14.h>


#define TRUE !0
#define FALSE 0

#define TxData 2	// PORTC, Bit 2
#define INV

void flash_led(byte num_flashes);
float ad_meas(byte channel);
void i2c_transmit_bytes(float v);
void setup_i2c_slave(void);

void start_timer(void);
void stop_timer(void);

byte i2c_int_occ, t0_num_ov, timeout_flag;

void main(void)
{
    byte dummy_byte, address_byte, command_byte;
    float v;

    ser_init();
    setup_i2c_slave();
    while(1)
    {

IDLE_STATE:

	   i2cen = 0;	// reset the I2C module
	   i2cen = 1;
	   i2c_int_occ = FALSE;
	   i2cie = 1;  peie = 1; gie = 1;

	   while(!i2c_int_occ)
	   {
#asm
		   CLRWDT
#endasm
	   // idle waiting for I2C interrupt
	   }

	   // character was received

	   i2c_int_occ = FALSE;
	   address_byte = I2CBUF;
	   ser_new_line();
	   ser_char('I');	// I for idle - used for debugging
       ser_hex_byte(I2CSTAT);
       ser_char(' ');
	   if ((stat_da == 0) && (stat_s))	// address state and start bit
	   {
		   if (stat_rw == 0)	// write to slave
		   {
			   goto WRITE_STATE;
		   }

		   else
		   {
			   goto READ_STATE;	// read from slave
		   }
	   }

	   else	// invalid first byte
	   {
		   goto IDLE_STATE;
	   }

WRITE_STATE:

       start_timer();
       while(!i2c_int_occ)
	   {
#asm
		   CLRWDT
#endasm
           if (timeout_flag)
           {
			   ser_char('T');	// for debugging
			   stop_timer();
			   goto IDLE_STATE;
		   }

	   }

	   stop_timer();

       i2c_int_occ = FALSE;

       ser_char('W');	// Write
       ser_hex_byte(I2CSTAT);
       ser_char(' ');

	   if ((stat_da) && (stat_p))	// data and stop
	   {
		   goto COMMAND_STATE;
	   }
	   else
	   {
		   goto IDLE_STATE;
       }

READ_STATE:
       i2c_transmit_bytes(v);
       goto IDLE_STATE;


COMMAND_STATE:
       while(gie)
       {
		   gie = 0;
	   }
       i2cie = 0;	// turn off i2c interrupt
       command_byte = I2CBUF;

       ser_char('C');	// Write
	   ser_hex_byte(command_byte);
       ser_char(' ');

       if ((command_byte & 0xf0) == 0x10)	// high nibble of 1 is LED flash
       {
		   flash_led(command_byte & 0x0f);
		   goto IDLE_STATE;
	   }
	   else if ((command_byte & 0xf0) == 0x20)
	   {
		   v = ad_meas(command_byte & 0x07);
		   goto IDLE_STATE;
	   }

	   else
	   {
		   goto IDLE_STATE;
	   }
   }
}

void i2c_transmit_bytes(float v)
{
   byte *p, n;

   p = (byte *) &v;

   start_timer();
   ser_char('R');
   for (n=0; n<4; n++)
   {
	   i2c_int_occ = FALSE;
	   I2CBUF = *(p+n);
	   ckp = 1;

	   while(!i2c_int_occ || stat_bf)
	   {
#asm
          CLRWDT
#endasm
          if (timeout_flag)
	      {
			  ser_char('T');
			  timeout_flag = FALSE;
		      goto i2c_transmit_bytes_DONE;
		  }

       }
       ser_hex_byte(I2CSTAT);
   }

i2c_transmit_bytes_DONE:

    stop_timer();
}

float ad_meas(byte channel)
{
	return(((float) channel) * 0.5);	// this is a stub for testing
}


void start_timer(void)
{
	t0_num_ov = 0;
	timeout_flag = FALSE;
	t0cs = 0;
	psa = 0;	// assign prescaler to timer 0
	ps2 = 0;
	ps1 = 1;
	ps0 = 1;	// 16 usec	- ~ 4 ms per rollover * 250 = nominally 1.0 secs

	TMR0 = 0x00;
	t0if = 0;
	t0ie = 1;
}

void stop_timer(void)
{
	t0if = 0;
	t0ie = 0;
	timeout_flag = FALSE;
}

void flash_led(byte num_flashes)
{
	byte n;
	trisc4 = 0;

	for (n=0; n<num_flashes; n++)
	{
		portc4 = 1;
		delay_ms(200);
		portc4 = 0;
		delay_ms(200);
	}
}

void setup_i2c_slave(void)
{
	byte dummy;

	smhog = 0; 	// disable clock stretch
	spgnda = 0;	// ground ref is Vss
	i2csel = 0;	// use trisc7, trisc6
	smbus = 0;	// disable sme bus

	trisc7 = 1;	trisc6 = 1;		// configure SCL and SDA as inputs
	I2CCON = (I2CCON & 0xf0) | 0x06;	// I2C slave, 7-bit

	i2cen = 1;
	i2cov = 0;	// ??
	wcol = 0;   // ??
    ckp = 1;	// no clock stretch
	I2CADD = 0x40;
	dummy = I2CBUF;
}


#int_i2c
i2c_int_handler(void)
{
	i2c_int_occ = TRUE;
}

#int_rtcc
timer0_int_handler(void)
{
	++t0_num_ov;
	if (t0_num_ov > 250)
	{
		timeout_flag = TRUE;
	}
}

#int_default
default_int_handler(void)
{
}

#include <delay.c>
#include <ser_14.c>


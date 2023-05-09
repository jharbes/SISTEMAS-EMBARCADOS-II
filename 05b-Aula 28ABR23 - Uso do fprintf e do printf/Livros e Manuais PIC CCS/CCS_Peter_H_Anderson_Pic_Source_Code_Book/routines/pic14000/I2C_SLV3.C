// I2C_SLV3.C PIC14000
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
void setup_i2c_slave(void);

byte i2c_int_occ;

void main(void)
{
    byte y;
    ser_init();

	setup_i2c_slave();

    y = I2CBUF;

    while(1)
    {
	   i2c_int_occ = FALSE;
	   i2cie = 1;  peie = 1; gie = 1;

	   while(!i2c_int_occ)
	   {
	   // idle waiting for I2C interrupt
	   }

	   i2c_int_occ = FALSE;
	   y = I2CBUF;

	   ser_hex_byte(y);
	   ser_char(' ');
	   ser_hex_byte(I2CSTAT);
	   ser_new_line();


	   while(!i2c_int_occ)
	   {

	   }

       y = I2CBUF;

       ser_hex_byte(y);
       ser_char(' ');
       ser_hex_byte(I2CSTAT);

       while(gie)
       {
	 	   gie = 0;
	   }

	   flash_led(y&0x0f);
   }

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
}


#int_i2c
i2c_int_handler(void)
{
	i2c_int_occ = TRUE;
}

#int_default
default_int_handler(void)
{
}

#include <delay.c>
#include <ser_14.c>


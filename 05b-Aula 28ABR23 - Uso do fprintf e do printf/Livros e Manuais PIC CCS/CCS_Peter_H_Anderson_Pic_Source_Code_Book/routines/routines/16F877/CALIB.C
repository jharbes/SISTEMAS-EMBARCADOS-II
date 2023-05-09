Program CALIB.C.

In many applications, it is desireable to permit the user to set a quantity which is then saved to EEPROM.

For example, I recently developed a temperature measurement system for agricultural grain bins by measuring the forward voltage across a diode.  Cables consisting of eight diodes spaced some six feet apart had already been installed in the bins.  I was concerned with that diodes may well vary from one to another and thus decided to use a cailbration potentiometer to permit the end user to "tweek" the measurement by -10.0 to 10.0 degrees.

The concept is that on boot, the A/D associated with the potentiometer is read and if it is near ground (adval < 23), the value previously stored in EEPROM is used in all subsequent calculations.  However, if, on boot, the A/D value is above ground (ad_val>=23), the ad_val is written to EEPROM, but the A/D is read and this value is used in all subsequent calculations.  Thus, in my case, the user may adjust the potentiometer until the temperature values measured by my system agreed with some reference they might have available.  Once satisfied, they would leave the potentiometer at that setting and reboot the processor.  The processor would then read the A/D and on finding it is above ground, would write this value to EEPROM.  The user would then turn power off and either set the pot to its low value or replace it wth a ground.  On all subsequent boots, the processor reads the pot A/D and on finding it near ground, it uses the value previously stored in EEPROM.

Note that A/D values in the range of 0 - 22 are not valid calibration values, leaving 1001 values in the range of 23 to 1023.  The actual value of the parameter you are adjusting may be calculated;

	q = ((float)(adval - 23)) /1000.0 * (highest - lowest) + lowest;

For example, in the example below, where the thermostat settings may be set between -40 and 150;

	q = ((float)(adval - 23))/1000.0 * (150.0 - (-40.0)) + (-40.0)

This general idea might be adapted to any similar measurments or it might be used to permit the user to set thermostat trip points or to adjust the duty cycle of a PWM output or to set the period of a PIC output.

Using a potentiometer in conjunction with the PICs A/D converters is inexpensive and is far easier for the end user than asking that they interface the PIC circuit with a laptop to set data which is peculiar to their installation.


// CALIB.C
//
// On boot, reads the value of a potentiometer on A/D CH 0.  If near ground,
// uses a value previously stored in EEPROM and ignores the potentiometer.
// Otherwise, writes the potentiometer value to EEPROM, but uses potentiomter
// value.
//
// In this example. the potentiometer is used to set a thermostat trip point
// in the range  of -40.0 to 150.0 degrees.
//
// copyright, Peter H. Anderson, Jan, '01

#case

#device PIC16F877 *=16 ICD=TRUE

#include <defs_877.h>
#include <lcd_out.h> // LCD and delay routines

#define TRUE !0
#define FALSE 0

unsigned long meas_pot(void);
float calc_therm_set(unsigned long adval);

void save_to_eeprom(byte adr, byte *p_dat, byte num_bytes);
void read_from_eeprom(byte adr, byte *p_dat, byte num_bytes);
byte read_data_eeprom(byte adr);
void write_data_eeprom(byte adr, byte d);

void main(void)
{
   byte use_eeprom_flag;
   long adval;
   float T_F;

   pcfg3 = 0; pcfg2 = 1; pcfg1 = 0; pcfg0 = 0;
   // config A/D for 3/0

   lcd_init();

   adval = meas_pot();     // read calibration potentiometer

   if (adval < 23)         // if near ground, use the value stored in eeprom
   {
       use_eeprom_flag = TRUE;
   }

   else
   {
       // save pot val to EEPROM, but use potentiometer for the value
       save_to_eeprom(0x30, (byte *) &adval, 2);
       use_eeprom_flag = FALSE;
   }

   while(1)    // continually
   {
       if (use_eeprom_flag)
       {
		   read_from_eeprom(0x30, (byte *) &adval, 2);
	   }

	   else
	   {
		   adval = meas_pot();
	   }

	   T_F = calc_therm_set(adval);
	   lcd_clr_line(0);
       printf(lcd_char, "T_F = %3.2f", T_F);

       delay_ms(500);
   }
}

unsigned long meas_pot(void)
{
	unsigned long adval;

	adfm = 1;   // right justified
	adcs1 = 1; adcs0 = 1; // internal RC

	adon=1;  // turn on the A/D
	chs2=0;  chs1=0;   chs0=0;
	delay_10us(10);      // a brief delay

	adgo = 1;
	while(adgo)    ;  // poll adgo until zero
	adval = ADRESH;
    adval = adval << 8 | ADRESL;
    return(adval);
}

float calc_therm_set(unsigned long adval)
{
	float T_F;
	unsigned long x;	// intermediate variable
    if (adval < 23)
    {
		T_F = -40.0;
	}
	else
	{
	    T_F = ((float) (adval - 23)) * 190.0/1000.0 - 40.0;	// -40 to 150 degrees
	}
	return(T_F);
}

void save_to_eeprom(byte adr, byte *p_dat, byte num_bytes)
{
   byte n;

   for (n=0; n<num_bytes; n++)
   {
	   write_data_eeprom(adr, *p_dat);
	   ++adr;
	   ++p_dat;
   }
}

void read_from_eeprom(byte adr, byte *p_dat, byte num_bytes)
{
   byte n;

   for (n=0; n<num_bytes; n++)
   {
	   *p_dat = read_data_eeprom(adr);
	   ++adr;
	   ++p_dat;
   }
}

byte read_data_eeprom(byte adr)
{
   byte retval;
   eepgd = 0;		// select data EEPROM
   EEADR=adr;
   rd=1;			// set the read bit
   retval = EEDATA;
   return(retval);
}

void write_data_eeprom(byte adr, byte d)
{
   eepgd = 0;		// select data EEPROM

   EEADR = adr;
   EEDATA = d;

   wren = 1;		// write enable
   EECON2 = 0x55;	// protection sequence
   EECON2 = 0xaa;

   wr = 1;		// begin programming sequence

   delay_ms(10);

   wren = 0;		// disable write enable
}

#include <lcd_out.c>

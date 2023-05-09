// Program 1820_2.C
//
// Reads 64-bit address from DS1820, saves to the 16F877's flash EEPROM
// and displays it on LCD.
//
// Uses 64-bit address to perform temperature measurement.  Data is
// is displayed on LCD
//
// 16F877                               DS1820
//
// PORTD0  ---------------------------- DQ (term 2)
//
// copyright, Peter H. Anderson, Georgetown, SC, Mar, '01

#case

#device PIC16F877 *=16 ICD=TRUE

#include <defs_877.h>
#include <lcd_out.h>
#include <_1_wire.h>

#define FALSE 0
#define TRUE !0

void display_data(byte *d, byte num_vals);

void put_flash_eeprom_byte(long adr, byte d);	// write a single byte to flash
void put_flash_eeprom_bytes(long adr, byte *d, byte num_vals); // write multiple bytes

byte get_flash_eeprom_byte(long adr);	// read single byte
void get_flash_eeprom_bytes(long adr, byte *d, byte num_vals);	// read multiple bytes

void ds1820_read_rom(byte sensor, byte *ser_num);	// fetch 8 byte serial number
void ds1820_make_temperature_meas(byte sensor, byte *ser_num, byte *result);
                           // using match ROM

void main(void)
{
   byte ser_num[8], result[9];

   lcd_init();

   pspmode = 0;	// configure parallel slave port as general purpose port


   ds1820_read_rom(0, ser_num); 	// read serial number from DS1820

   printf(lcd_char, "Serial Number");
   delay_ms(1000);

   lcd_init();
   display_data(ser_num, 8);		// display the result on LCD
   delay_ms(2000);

   put_flash_eeprom_bytes(0x1000, ser_num, 8);
		// save to flash eeprom, beginning at adr 0x1000, 8 bytes

   // now fetch the serial number, address and continually perform temperature
   // measurments

   lcd_init();
   printf(lcd_char, "Now Measuring");
   delay_ms(1000);

   while(1)
   {
	   lcd_init();
	   get_flash_eeprom_bytes(0x1000, ser_num, 8);
                // fetch from flash ROM, 8 bytes and return in array ser_num

       ds1820_make_temperature_meas(0, ser_num, result);
       display_data(result, 9);		// display the 9 byte temperature result
       delay_ms(2000);
   }
}

void display_data(byte *d, byte num_vals)
{
  byte n, v, line = 0;

  lcd_clr_line(line);

  for (n=0; n<num_vals; n++)
  {
    v = d[n];		// intermediate variable used for debugging

    if ((n%4 == 0) && (n!=0))
	{
	   ++line;
	   if (line == 4)
	   {
		   line = 0;
	   }
	   lcd_clr_line(line);
	}

	lcd_hex_byte(v);
    lcd_char(' ');
  }
}

void put_flash_eeprom_bytes(long adr, byte *d, byte num_vals)
{
   byte n;
   for(n=0; n<num_vals; n++, adr++)
   {
      put_flash_eeprom_byte(adr, d[n]);
   }
}


void get_flash_eeprom_bytes(long adr, byte *d, byte num_vals)
{
   byte n;
   for(n=0; n<num_vals; n++, adr++)
   {
      d[n]=get_flash_eeprom_byte(adr);
   }
}

void put_flash_eeprom_byte(long adr, byte d)
{
   EEADRH = adr >> 8;
   EEADR = adr & 0xff;

   EEDATH = d >> 8;
   EEDATA = d & 0xff;
   eepgd = 1;    // program memory
   wren = 1;
   EECON2 = 0x55;
   EECON2 = 0xaa;
   wr = 1;
#asm
   NOP
   NOP
#endasm
   wren = 0;
}

byte get_flash_eeprom_byte(int adr)
{

   EEADR = adr;
   EEADRH = (adr>>8);
   eepgd = 1;
   rd = 1;
#asm
   NOP
   NOP
#endasm
   return(EEDATA);
}

void ds1820_read_rom(byte sensor, byte *ser_num)
{
   byte n, v;

   _1w_init(sensor);
   _1w_out_byte(sensor, 0x33);	// "Read ROM" command

   for(n=0; n<8; n++)
   {
      v =_1w_in_byte(sensor);
      ser_num[n] = v;		// intermediate variable used for debugging
   }
}

void ds1820_make_temperature_meas(byte sensor, byte *ser_num, byte *result)
{
   byte n;

   _1w_init(sensor);
   _1w_out_byte(sensor, 0x55);	// match ROM
   for(n=0; n<8; n++)	// followed by the 8-byte ROM address
   {
      _1w_out_byte(sensor, ser_num[n]);
   }

   _1w_out_byte(sensor, 0x44);	// start temperature conversion
   _1w_strong_pull_up(sensor);

   _1w_init(sensor);
   _1w_out_byte(sensor, 0x55);	// match ROM
   for(n=0; n<8; n++)	// followed by the 8-byte ROM address
   {
      _1w_out_byte(sensor, ser_num[n]);
   }
   _1w_out_byte(sensor, 0xbe);	// fetch temperature data (nine bytes)

   for(n=0; n<9; n++)
   {
      result[n]=_1w_in_byte(sensor);
   }
}

#include <lcd_out.c>
#include <_1_wire.c>

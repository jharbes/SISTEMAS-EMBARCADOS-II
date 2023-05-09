// EE_SAVE.C
//
// Illustrates how to save a quantity to and fetch a quantity from EEPROM.
//
// Saves a float and a struct TM to EEPROM and then fetches them and displays
// on LCD.
//
// Note that a byte pointer which points to the beginning of the quantity is passed
// to each function.
//
// copyright, Peter H. Anderson, Baltimore, MD, Jan, '01

#case

#device PIC16F877 *=16 ICD=TRUE

#include <defs_877.h>
#include <lcd_out.h>

void save_to_eeprom(byte adr, byte *p_dat, byte num_bytes);
void read_from_eeprom(byte adr, byte *p_dat, byte num_bytes);

void write_data_eeprom(byte adr, byte d);
byte read_data_eeprom(byte adr);

struct TM
{
	byte hr;
	byte mi;
	byte se;
};

void main(void)
{
   float float_1 = 1.2e-12, float_2;
   struct TM t1, t2;
   byte *ptr;

   lcd_init();
   t1.hr = 12;	t1.mi = 45; 	t1.se = 33;

   ptr = (byte *) &float_1;		// ptr points to first byte of float_1
   save_to_eeprom(0x00, ptr, sizeof(float));	// save flaot_1

   ptr = (byte *) &t1;
   save_to_eeprom(0x10, ptr, sizeof(struct TM));	// save t1

   ptr = (byte *) &float_2;
   read_from_eeprom(0x00, ptr, sizeof(float));

   ptr = (byte *) &t2;
   read_from_eeprom(0x10, ptr, sizeof(struct TM));

   lcd_clr_line(0);				// print the float
   printf(lcd_char, "float = %1.3e", float_2);

   lcd_clr_line(1);				// print the time
   printf(lcd_char, "t2 = ");
   lcd_dec_byte(t2.hr, 2);
   lcd_char(':');
   lcd_dec_byte(t2.mi, 2);
   lcd_char(':');
   lcd_dec_byte(t2.se, 2);

   while(1)
#asm
   CLRWDT
#endasm
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
   rd=1;	// set the read bit
   retval = EEDATA;
#ifdef TEST
   lcd_cursor_pos(0, 15);
   printf(lcd_char, "%x %x", adr, retval);
   delay_ms(2000);
#endif
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


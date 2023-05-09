// FIRST_TM.C
//
// Illustrates the use of data EEPROM on the PIC16F87X.
//
// On download, EEPROM locations 0x00 - 0x03 are initialized to a distinctive
// pattern (0x5a, 0xa5, 0x5a, 0xa5).
//
// Each time the program is executed, function is_first_time() is called. If
// the distinctive pattern is detected, the locations are changed to 0x00 and
// TRUE is returned.  Subsequent calls will return FALSE.
//
// If, it is the first time, an 8-byte serial number is read from an external
// device and written to EEPROM locations 0x10 - 0x17.  The external device  might
// be a Dallas DS2401 1-W Silicon Serial Number. (In this routine, a stub is used
// to pass back a serial number).  Thus, the PIC is now married to a unique piece
// of hardware.
//
// Each time the program is executed, the 8-byte serial number is read from the
// external DS2401 and compared with that stored in EEPROM.  If they agree, the
// task (flashing of an LED) is executed.  If not, the program is locked.
//
// As noted, the fetching of the serial number from the DS2401 is actually
// implemented in software.  Note that on the fifth call to this routine, an error
// is introduced which causes the program to lock.  This is facilitated by using
// EEPROM location 0x08 as a persistent location for variable num_calls.
//
// copyright, Peter H. Anderson, Baltimore, MD, Dec, '00

#case

#device PIC16F877 *=16 ICD=TRUE
#include <defs_877.h>
#include <lcd_out.h>

#define TRUE !0
#define FALSE 0

//#define TEST

byte is_first_time(void);	// returns true if locations 0x00 - 0x03 in EEPROM are
							// at specific first time values.  Id so, sets these
							// locations to 0x00

byte is_valid_ser_num(void);// tests if _2401 ser number agrees with EEPROM locations
							// 0x10 - 0x17

void _2401_fetch_ser_num(byte *ser_num); // returns a serial number.  This is a stub.
void write_eeprom_ser_num(byte *ser_num);// writes serial number to locations 0x00 - 0x17

void write_data_eeprom(byte adr, byte d);
byte read_data_eeprom(byte adr);

void flash_led(void);

void main(void)
{
   byte ser_num[8];

   pspmode = 0;

   portd4 = 0;		// LED
   trisd4 = 0;

   lcd_init();

   if (is_first_time())		// if the first time, fetch serial number
   							// from DS2401 and save to data EEPROM
   {
	   _2401_fetch_ser_num(ser_num);	// fetch ser num from DS2401
	   write_eeprom_ser_num(ser_num);	// and save.
	   write_data_eeprom(0x08, 0x00);	// zero the number of calls to _2401_fetch
   }

   if (is_valid_ser_num())	// if DS2401 and EEPROM ser nums agree
   {
	   lcd_clr_line(0);
	   printf(lcd_char, "Valid");
	   while(1)
	   {
		  flash_led();
	   }
   }

   else		// there was no match.  Lock the system.
   {
       lcd_clr_line(0);
	   printf(lcd_char, "Invalid");
	   lcd_clr_line(1);
       printf(lcd_char, "System Locked");
	   while(1)
	   {
#asm
		  CLRWDT
#endasm
  	   }
   }
}

void write_eeprom_ser_num(byte *ser_num)
{
	byte n;

	for (n=0; n<8; n++)
	{
		write_data_eeprom(n+0x10, ser_num[n]);
	}
}

byte is_valid_ser_num(void)
{
   byte n, ser_num[8];

   _2401_fetch_ser_num(ser_num);	// fetch ser num from DS2401
   for (n = 0; n<8; n++)
   {
	   if(read_data_eeprom(n+0x10) != ser_num[n])	// if not the same
	   {
		   return(FALSE);
	   }
   }
   return(TRUE);					// all eight bytes matched
}

void _2401_fetch_ser_num(byte *ser_num)	// this is a stub
{
	byte n, num_calls;

	const byte _2401_ser_num[8] = {0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00};


	for (n=0; n<8; n++)
   {
     ser_num[n] = _2401_ser_num[n];
   }

   num_calls = read_data_eeprom(0x08);
   ++num_calls;
   write_data_eeprom(0x08, num_calls);

   lcd_clr_line(3);
   printf(lcd_char, "Num Calls = %d", num_calls);

   if (num_calls == 5)		// on the 5th call, introduce an erro
   {
	   ser_num[3] = 0x78;	// make serial number incorrect
   }
}

byte is_first_time(void)
{

   byte n;
   const byte x[4] = {0x5a, 0xa5, 0x5a, 0xa5};
   for (n = 0; n<4; n++)
   {
      if (read_data_eeprom(n) != x[n])
      {
 	     return(FALSE);
	  }
   }

   for (n=0; n<4; n++)	// is it is first time, write 0x00s to each location
   {
		write_data_eeprom(n, 0x00);
#ifdef TEST
        read_data_eeprom(n);
#endif
   }

   return(TRUE);
}


void flash_led(void)
{
   portd4 = 1;
   delay_ms(250);
   portd4 = 0;
   delay_ms(250);
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


#rom 0x2100={0x5a, 0xa5, 0x5a, 0xa5}



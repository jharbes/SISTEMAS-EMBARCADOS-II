// EEPROM_1.C
//
// Illustrates how to initialize EEPROM, how to read from EEPROM and
// write to EEPROM.  Note that this EEPROM is the data EEPROM on the 16F87X.
//
// EEPROM location 00 is initialized to 100 decimal using the #rom
// directive.  Each time function dec_count is called, the program
// decrements this value and checks to see if it is at zero.  Such an
// arrangement might be used to limit the number of accesses and might
// be used with a debit card.
//
// Program continually flashes LED on PORTD4 at about 250 ms on and 250 msec
// off.  Loops indefinitely.  However, counter in EEPROM is decremented on
// each pass.  Program locks when the EEPROM counter is decremented to zero.
//
// Note that even if power is turned off prior to the completion of 100
// flashes, the latest EEPROM value will be retained for the subsequent
// run of the program.
//
// copyright, Peter H. Anderson, Baltimore, MD, Dec, '00

#case

#device PIC16F877 *=16 ICD=TRUE
#include <defs_877.h>
#include <lcd_out.h>

#define TRUE !0
#define FALSE 0

// #define INTS

void flash_led(void);
void write_data_eeprom(byte adr, byte d);
byte read_data_eeprom(byte adr);

#ifdef INTS
byte ee_int_occ;
#endif

void main(void)
{
   byte n;

   pspmode = 0;

   portd4 = 0;		// LED
   trisd4 = 0;

   lcd_init();

   while(1)
   {
      n = read_data_eeprom(0x00);	// fetch from EEPROM
      if (n==0)
      {
         break;						// if at zero, lock up
	  }
      lcd_cursor_pos(0, 0);
      printf(lcd_char, "%u   ", n);

      flash_led();					// falsh the LED one time

      --n;
      write_data_eeprom(0x00, n);	// decrement and save
   }

   lcd_clr_line(0);
   printf(lcd_char, "Locked!!!");

#asm		// lock when EEPROM value is decremented to zero
LOCK:
   CLRWDT
   GOTO LOCK
#endasm
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
   eepgd = 0;		// select data EEPROM
   EEADR=adr;
   rd=1;	// set the read bit
   return(EEDATA);
}

void write_data_eeprom(byte adr, byte d)
{
   eepgd = 0;		// select data EEPROM
#ifdef INTS
   ee_int_occ = FALSE;
   while(gie)		// be sure interrupts are off while executing the key
   {
	   gie = 0;
   }

   eeif = 0;
   eeie = 1;
   peie = 1;
#endif
   EEADR = adr;
   EEDATA = d;

   wren = 1;		// write enable
   EECON2 = 0x55;	// protection sequence
   EECON2 = 0xaa;

   wr = 1;		// begin programming sequence

#ifdef INTS
   gie = 1;
   while(!ee_int_occ)		;
   ee_int_occ = FALSE;
#else
   delay_ms(10);
#endif

   wren = 0;		// disable write enable
}

#include <lcd_out.c>

#ifdef INTS

#int_eeprom eeprom_int_handler(void)
{
   ee_int_occ = TRUE;
}

#int_default default_int_handler(void)
{
}
#endif


#rom 0x2100={100}	// intialize location 0 to 100




#ifdef X

For example, a float and three bytes might be saved;

   float fl = 1.23e4
   byte a = 0, b = 90, c = 100, *p;
   byte dat[8];

   p = (byte *) &fl;
   dat[0] = *p;
   dat[1] = *(p+1);
   dat[2] = *(p+2);
   dat[3] = *(p+3);

   dat[4] = a;
   dat[5] = b;
   dat[6] = c;

   write_flash_block(dat);

   //....

   read_flash_block(dat)

   p = (byte *) &fl;

   *p = dat[0];
   *(p+1) = dat[1];
   *(p+2) = dat[2];
   *(p+3) = dat[3];

   a = dat[4];
   b = dat[5];
   c = dat[6];

#endif

// Program FLSH_EE.C
//
// Illustrates how to write and read a block of eight bytes to flash
// EEPROM.  After 256 writes to the same eight locations, the address is
// changed to eight different locations, and this is repeated 32 times and
// the process is repeated.  This permits variables to be saved 32 times
// as often as if the same eight locations were used.
//
// copyright, Peter H. Anderson, Baltimore, MD, May, '01

#case

#device PIC16F877 *=16 ICD=TRUE

#include <defs_877.h>
#include <lcd_out.h> // LCD and delay routines

#define TRUE !0
#define FALSE 0

#define BLOCK_NUM_ADR 0x1cff
#define BASE_COUNTER_ADR 0x1c00
#define BASE_STORE_ADR 0x1d00

#define TEST

byte read_block_number(void);
write_block_number(byte block_num);
byte read_counter(byte block_num);
write_counter(byte block_num, counter);
read_flash_block(byte *d);
write_flash_block(byte *d);

long get_flash_eeprom(long adr);
void put_flash_eeprom(long adr, long dat);

byte is_first_time(void); // returns true if locations 0x00 - 0x03 in EEPROM are
       // at specific first time values.  If so, sets these
       // locations to 0x00


void write_data_eeprom(byte adr, byte d);
byte read_data_eeprom(byte adr);

void main(void)
{
   byte n, dat[8], block_num, counter;
   long num = 0;

   lcd_init();

   if (is_first_time())
   {
      write_block_number(0x00);
      for (n=0; n< 32; n++)
      {
         write_counter(n, 0xff); // zero all of the counters
      }
      dat[0] = 1;
      // leave others as garbage
      write_flash_block(dat);
   }

   while(1)
   {
      read_flash_block(dat);
      if ((num%100) == 0)
      {
         lcd_clr_line(0);
         block_num = read_block_number();
         counter = read_counter(block_num);
         lcd_clr_line(0);
         printf(lcd_char, "Blk = %u", block_num);
         lcd_clr_line(1);
         printf(lcd_char, "Counter = %u", counter);
         lcd_clr_line(2);
         printf(lcd_char, "Data = %u", dat[0]);
         delay_ms(1000);
      }
      ++dat[0];
      write_flash_block(dat);
      ++num;
   }
}

byte read_block_number(void)
{
    byte block_number;
    block_number = (byte) get_flash_eeprom(BLOCK_NUM_ADR);
    return(block_number);
}

write_block_number(byte block_num)
{
    put_flash_eeprom(BLOCK_NUM_ADR, (long) block_num);
}

byte read_counter(byte block_num)
{
    byte counter;
    long adr;

    adr = BASE_COUNTER_ADR + block_num;
    counter = get_flash_eeprom(adr);
    return(counter);
}

write_counter(byte block_num, counter)
{
    long adr;
    adr = BASE_COUNTER_ADR + block_num;
    put_flash_eeprom(adr, (long) counter);
}

write_flash_block(byte *d)
{
    byte block_num, n, counter;
    long adr;

    block_num = read_block_number();
    counter = read_counter(block_num);
    ++counter;
    if (counter == 0xff)
    {
      ++block_num;
      if (block_num > 31)
      {
          block_num = 0;
      }
      write_block_number(block_num);
   }
   write_counter(block_num, counter);
   adr = (long) block_num * 8 + BASE_STORE_ADR;
   for (n = 0; n<8; n++)
   {
       put_flash_eeprom(adr, (long) d[n]);
       ++adr;
   }
}

read_flash_block(byte *d)
{
    byte block_num, n;
    long adr;

    block_num = read_block_number();
    adr = (long) block_num * 8 + BASE_STORE_ADR;
    for (n = 0; n<8; n++)
    {
       d[n] = (byte)get_flash_eeprom(adr);
       ++adr;
    }
}

void put_flash_eeprom(long adr, long dat)
{
    while(gie)       // be sure interrupts are disabled
    {
        gie = 0;
    }
    EEADRH = adr >> 8;
    EEADR = adr & 0xff;

    EEDATH = dat >> 8;
    EEDATA = dat & 0xff;
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
    gie = 1;
}

long get_flash_eeprom(long adr)
{
    long eeprom_val;
    EEADRH = adr >> 8;
    EEADR = adr & 0xff;
    eepgd = 1;
    rd = 1;
#asm
    NOP
    NOP
#endasm
    eeprom_val = EEDATH;
    eeprom_val = eeprom_val << 8 | EEDATA;
    return(eeprom_val);
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

   for (n=0; n<4; n++) // is it is first time, write 0x00s to each location
   {
      write_data_eeprom(n, 0x00);
#ifdef TEST
      read_data_eeprom(n);
#endif
   }

   return(TRUE);
}

byte read_data_eeprom(byte adr)
{
   byte retval;
   eepgd = 0;  // select data EEPROM
   EEADR=adr;
   rd=1; // set the read bit
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
   eepgd = 0;  // select data EEPROM

   EEADR = adr;
   EEDATA = d;

   wren = 1;  // write enable
   EECON2 = 0x55; // protection sequence
   EECON2 = 0xaa;

   wr = 1;  // begin programming sequence

   delay_ms(10);

   wren = 0;  // disable write enable
}


#include <lcd_out.c>

#rom 0x2100={0x5a, 0xa5, 0x5a, 0xa5}



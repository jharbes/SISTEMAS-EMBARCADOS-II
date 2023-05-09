// DS2435_3.C (PIC16F877)
//
// Illustrates how to use EEPROM and static RAM associated with the DS2435.
// Write the string "Morgan State Univeristy" to the 24 bytes of EEPROM on page
// 1, writes two floats to the eight EEPROM bytes on page 2 and 32 bytes to SRAM
// on page 3.
//
// Then reads this data back and displays on LCD.
//
// PIC16F877								DS2435
//										+5 VDC --
// PORTD0 (term 19) ----------------------------- DQ
//										GRD -----
//
// Note that a 4.7K pullup resistor to +5 VDC is on the DQ lead.
//
// This was developed by Ernest N. Wells, Jr. as a part of his Senior Project
// at Morgan State University.
//
// copyright, Peter H. Anderson, Baltimore, MD, May, '01

#case

#device PIC16F877 *=16 ICD=TRUE
#include <a:\defs_877.h>
#include <a:\lcd_out.h>
#include <a:\_1_wire.h>

#define FALSE 0
#define TRUE !0

void clear_2435_scratchpad_all(void);
void write_2435_scratchpad(byte adr, byte *a, byte num_vals);
void read_2435_scratchpad(byte adr, byte *a, byte num_vals);

void transfer_2435_scratchpad_to_mem(byte page);
void transfer_2435_mem_to_scratchpad(byte page);

void main(void)
{
   byte const a1[24] = {"Morgan State University"};
   byte a[8], n, x;
   float f1 = 1.23e-6, f2 = 17.3e-6;

   lcd_init();
   pspmode = 0;

   for (n=0; n<8; n++)  // write to page 1, eight bytes at a time
   {
     a[n] = a1[n];
   }
   write_2435_scratchpad(0x00, a, 8);

   for (n=0; n<8; n++)
   {
     a[n] = a1[n+8];
   }
   write_2435_scratchpad(0x08, a, 8);

   for (n=0; n<8; n++)
   {
     a[n] = a1[n+16];
   }
   write_2435_scratchpad(0x10, a, 8);

   transfer_2435_scratchpad_to_mem(1);

   write_2435_scratchpad(0x20, &f1, 4); // write to page 2
   write_2435_scratchpad(0x24, &f2, 4);
   transfer_2435_scratchpad_to_mem(2);

   for (n=0; n<32; n++)     // write to page 3
   {
      x = 255 - n;
      write_2435_scratchpad(0x40+n, &x, 1);
   }
   transfer_2435_scratchpad_to_mem(3);

   clear_2435_scratchpad_all();

   // Now read the data back and display
   transfer_2435_mem_to_scratchpad(1);
   transfer_2435_mem_to_scratchpad(2);
   transfer_2435_mem_to_scratchpad(3);

   printf(lcd_char, "Page 1");

   lcd_clr_line(1);
   read_2435_scratchpad(0x00, a, 8);
   for (n=0; n<8; n++)
   {
      lcd_char(a[n]);
   }

   lcd_clr_line(2);
   read_2435_scratchpad(0x08, a, 8);
   for (n=0; n<8; n++)
   {
      lcd_char(a[n]);
   }

   lcd_clr_line(3);
   read_2435_scratchpad(0x10, a, 8);
   for (n=0; n<8; n++)
   {
      lcd_char(a[n]);
   }

   delay_ms(1000);

   lcd_init();
   printf(lcd_char, "Page 2 - floats");
   read_2435_scratchpad(0x20, &f1, 4);
   read_2435_scratchpad(0x23, &f2, 4);

   lcd_clr_line(1);
   printf(lcd_char, "%e", f1);

   lcd_clr_line(2);
   printf(lcd_char, "%e", f2);

   delay_ms(1000);

   lcd_init();
   printf(lcd_char, "Page 3");

   for (n=0x40; n<0x60; n++)
   {
     lcd_clr_line(1);
     read_2435_scratchpad(n, &x, 1);
     lcd_dec_byte(x, 3);
     delay_ms(100);
   }
   while(1)          ;
}

void clear_2435_scratchpad_all(void)
{
   byte n;
   _1w_init(0);
   _1w_out_byte(0, 0x17);
   _1w_out_byte(0, 0x00);
   for (n=0; n<0x5f; n++)
   {
      _1w_out_byte(0, 0x00);
   }
}

void write_2435_scratchpad(byte adr, byte *a, byte num_vals)
{
    byte n;

   _1w_init(0);
   _1w_out_byte(0, 0x17);
   _1w_out_byte(0, adr);
   for (n=0; n<num_vals; n++)
   {
     _1w_out_byte(0, a[n]);
   }
}

void read_2435_scratchpad(byte adr, byte *a, byte num_vals)
{
   byte n;

   _1w_init(0);
   _1w_out_byte(0, 0x11);
   _1w_out_byte(0, adr);
   for (n=0; n<num_vals; n++)
   {
     a[n] = _1w_in_byte(0);
   }
}

void transfer_2435_scratchpad_to_mem(byte page)
{
    byte const command[4] = {0x00, 0x22, 0x25, 0x28}; // zeroth element not used

    if (page == 1)
    {
       _1w_init(0);
       _1w_out_byte(0, 0x44);  // unlock
    }

    _1w_init(0);
    _1w_out_byte(0, command[page]);
    delay_ms(50);

    if (page == 1)
    {
       _1w_init(0);
       _1w_out_byte(0, 0x43);  // lock
    }
}

void transfer_2435_mem_to_scratchpad(byte page)
{
    byte const command[4] = {0x00, 0x71, 0x77, 0x7a}; // zeroth ele not used

    _1w_init(0);
    _1w_out_byte(0, command[page]);
    delay_ms(10);
}

#include <a:\lcd_out.c>
#include <a:\_1_wire.c>

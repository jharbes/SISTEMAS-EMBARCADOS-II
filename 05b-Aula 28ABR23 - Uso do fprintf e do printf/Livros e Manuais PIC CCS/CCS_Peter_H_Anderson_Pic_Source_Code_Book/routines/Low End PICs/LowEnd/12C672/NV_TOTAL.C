// Program NV_TOTAL.C (12CE674), CCS PCB
//
// An implemenation of a non-volatile totalizer.
//
// On boot checks GP3 input.  If at ground, sets four bytes in EEPROM to zero.
//
// Uses TMR0 to count external events.  On rollover, an interrupt occurs and 256
// is added to the four byte quantity stored in EEPROM.
//
// A change on input GP1, such as a PC sending a character, causes the program to
// read the 4-byte quantity from EEPROM, add whatever residual count is in the
// TMR0 counter and send the 32 bit quantity to the PC or similar in hex format.
//
// An application might be an outboard processor to monitor the counts of a tipping
// bucket in a rain fall measurement device.
//
// Note that on power failure, only residual content of TMR0 is lost.  In this
// example, the full 256 count of TMR0 is used.  This might be reduced as discussed
// in the text.
//
//         PIC12C672
//
// Count Source ----------------- GP2 (T0CKI) Use external pullup if necessary.
//
//              ----------------- GP3 (/CLEAR)
// PC Com Port  ---- 22K -------- GP1 (Send)
//          GP0 ----------------> To PC Com Port
//
// copyright, Peter H. Anderson, Elmore, VT, July, '01

#case

#device PIC12CE674

#include <defs_672.h>
#include <string.h> // for strcpy

#include <delay.h>
#include <ser_672.h>

#define TxData 0 // use GP0
#define INV // send inverted RS232

#define TRUE !0
#define FALSE 0

struct long32
{
   unsigned long h;
   unsigned long l;
};

void add_long_to_count32(struct long32 *p_q, long v);
void write_count32(struct long32 *p_q);
void read_count32(struct long32 *p_q);

byte i2c_internal_eeprom_random_read(byte adr);
void i2c_internal_eeprom_random_write(byte adr, byte dat);

// standard I2C routines for internal EEPROM
byte i2c_internal_in_byte(byte ack);
void i2c_internal_out_byte(byte o_byte);

void i2c_internal_start(void);
void i2c_internal_stop(void);
void i2c_internal_high_sda(void);
void i2c_internal_low_sda(void);
void i2c_internal_high_scl(void);
void i2c_internal_low_scl(void);

byte high_two_bits; // bits 7 and 6 of GPIO
byte tmr0_int_occ, gpio_change_int_occ;

void main(void)
{
   byte x, current_count;
   struct long32 count32;

//   calibrate();  // do not use this function during emulation

   pcfg2 = 1;; // GP0, GP1, GP2, GP4 configured as general purpose IOs
   pcfg1 = 1;
   pcfg0 = 1;

   not_gppu = 0; // enable weak pullups

   ser_init();

   high_two_bits = 0xc0; // bits 7 and 6 at one
   GPIO = GPIO & 0x3f | high_two_bits;

   if (!gp3)  // if on boot, the clear input is at zero
   {
      count32.h = 0x0000;
      count32.l = 0x0000;
      write_count32(&count32);
   }

   gp3 = 0;  // make it an output 0 to avoid any int on change
   tris3 = 0;

   // configure TMR0
   t0cs = 1;  // input on T0CKI (GP2)
   t0se = 1;
   ps2 = 0;
   ps1 = 0;
   ps0 = 0;
   psa = 1;  // prescaler assigned to watch dog timer
   TMR0 = 0;
   t0if = 0;
   t0ie = 1;

   // config for int on change
   gpif = 0;
   gpie = 1;
   x = GPIO;

   tmr0_int_occ = FALSE;
   gpio_change_int_occ = FALSE;

   while(1)
   {
      gie = 1;  // enable interrupts
      if (tmr0_int_occ) // there was a roll over
      {
         read_count32(&count32);
         add_long_to_count32(&count32, 256);
         write_count32(&count32);
         tmr0_int_occ = FALSE;
      }

      if (gpio_change_int_occ) // it must be the GO lead
      {
         while(gie)
         {
            gie = 0;
         }
         current_count = TMR0;  // fetch the residual count in TMR0
         TMR0 = 0x00;
         read_count32(&count32);
         add_long_to_count32(&count32, (long) current_count); // add any residual
         write_count32(&count32);

         x = (byte)(count32.h >> 8);
         ser_hex_byte(x);
         x = (byte)(count32.h);
         ser_hex_byte(x);

         x = (byte)(count32.l >> 8);
         ser_hex_byte(x);
         x = (byte)(count32.l);
         ser_hex_byte(x);

         ser_new_line();
         gpio_change_int_occ = FALSE;

         gpif = 0;
         gie = 1;
      }
   }
}

void add_long_to_count32(struct long32 *p_q, long v) // adds v to the 32 bit structure
{
   unsigned long old;
   old = p_q->l;
   p_q->l = p_q->l + v;
   if (p_q->l < old) // there was an overflow
   {
      ++(p_q->h);
   }
}

void write_count32(struct long32 *p_q)  // save the 32-bit quantity to EEPROM
{
   byte adr, *p_byte;
   p_byte = (byte *) p_q; // p_byte now points to beginning of structure
   for (adr = 0; adr < 4; adr++)
   {
      i2c_internal_eeprom_random_write(adr, *(p_byte + adr));
   }
}

void read_count32(struct long32 *p_q)  // read the 32-bit quantity
{
   byte adr, *p_byte;
   p_byte = (byte *) p_q; // p_byte now points to beginning of structure
   for (adr = 0; adr < 4; adr++)
   {
      *(p_byte +adr) = i2c_internal_eeprom_random_read(adr);
   }
}


byte i2c_internal_eeprom_random_read(byte adr)
{
   byte d;
   i2c_internal_start();
   i2c_internal_out_byte(0xa0);
   i2c_internal_out_byte(adr);

   i2c_internal_start();
   i2c_internal_out_byte(0xa1);
   d = i2c_internal_in_byte(0);  // no ack prior to stop
   i2c_internal_stop();
   return(d);
}

void i2c_internal_eeprom_random_write(byte adr, byte dat)
{
   i2c_internal_start();
   i2c_internal_out_byte(0xa0);
   i2c_internal_out_byte(adr);
   i2c_internal_out_byte(dat);
   i2c_internal_stop();
   delay_ms(25); // wait for byte to burn
}

byte i2c_internal_in_byte(byte ack)
{
   byte i_byte, n;
   i2c_internal_high_sda();
   for (n=0; n<8; n++)
   {
      i2c_internal_high_scl();
      if (sda_in)
      {
         i_byte = (i_byte << 1) | 0x01; // msbit first
      }
      else
      {
         i_byte = i_byte << 1;
      }
      i2c_internal_low_scl();
   }
   if (ack)
   {
    i2c_internal_low_sda();
   }
   else
   {
    i2c_internal_high_sda();
   }
   i2c_internal_high_scl();
   i2c_internal_low_scl();

   i2c_internal_high_sda();
   return(i_byte);
}

void i2c_internal_out_byte(byte o_byte)
{
   byte n;
   for(n=0; n<8; n++)
   {
      if(o_byte&0x80)
      {
         i2c_internal_high_sda();
         //ser_char('1'); // used for debugging
      }
      else
      {
         i2c_internal_low_sda();
         //ser_char('0'); // used for debugging
      }
      i2c_internal_high_scl();
      i2c_internal_low_scl();
      o_byte = o_byte << 1;
   }
   i2c_internal_high_sda();

   i2c_internal_high_scl(); // provide opportunity for slave to ack
   i2c_internal_low_scl();
   //ser_new_line();   // for debugging
}

void i2c_internal_start(void)
{
   i2c_internal_low_scl();
   i2c_internal_high_sda();
   i2c_internal_high_scl(); // bring SDA low while SCL is high
   i2c_internal_low_sda();
   i2c_internal_low_scl();
}

void i2c_internal_stop(void)
{
   i2c_internal_low_scl();
   i2c_internal_low_sda();
   i2c_internal_high_scl();
   i2c_internal_high_sda();  // bring SDA high while SCL is high
   // idle is SDA high and SCL high
}

void i2c_internal_high_sda(void)
{
   high_two_bits = high_two_bits | 0x40; // X1
   GPIO = (GPIO & 0x3f) | high_two_bits;
   delay_10us(5);
}

void i2c_internal_low_sda(void)
{
   high_two_bits = high_two_bits & 0x80; // X0
   GPIO = (GPIO & 0x3f) | high_two_bits;
   delay_10us(5);
}

void i2c_internal_high_scl(void)
{
   high_two_bits = high_two_bits | 0x80; // 1X
   GPIO = (GPIO & 0x3f) | high_two_bits;
   delay_10us(5);
}

void i2c_internal_low_scl(void)
{
   high_two_bits = high_two_bits & 0x40; // 0X
   GPIO = (GPIO & 0x3f) | high_two_bits;
   delay_10us(5);
}

void calibrate(void)
{
#asm
   CALL 0x03ff
   MOVWF OSCCAL
#endasm
}

#int_rb gpio_change_int_handler(void)
{
   byte x;
   x = GPIO;
   if (x & 0x02)  // if GP1 is at a logic one
   {
      gpio_change_int_occ = TRUE;
   }
}

#int_rtcc tmr0_int_handler(void)
{
   tmr0_int_occ = TRUE;
}

#int_default default_handler(void)
{
}

#include <delay.c>
#include <ser_672.c>




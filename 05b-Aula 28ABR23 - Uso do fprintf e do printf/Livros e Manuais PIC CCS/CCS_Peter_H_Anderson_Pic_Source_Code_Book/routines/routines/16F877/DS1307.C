// DS1307.C
//
// Writes a base time and date to DS1307.  About every second reads
// time and date and displays to serial LCD on RA.0.
//
// Offers a good example of working with structures.  Note that
// structures may only be passed using pointers.
//
//    DS1307                         DS1307
//
// SCL (term 18)------------------- SCL (term 6) ----- To Other
// SDA (term 23) ------------------- SDA (term 5) ----- I2C Devices
//
// Note that there is no provision for the user defining the
// secondary I2C address using straps.
//
// copyright, Peter H. Anderson, Baltimore, MD, Mar, '01

#case

#device PIC16F877 *=16 ICD=TRUE

#include <defs_877.h>
#include <lcd_out.h>
#include <i2c_mstr.h>

#define TRUE !0
#define FALSE 0

struct Time
{
   byte hr;
   byte mi;
   byte se;
};

struct Date
{
   byte yr;
   byte mo;
   byte da;
   byte weekday;
};

// routines used for DS1307
void set_up_clock(int control_reg);
void write_clock(struct Time *p_t, struct Date *p_d);
void read_clock(struct Time *p_t, struct Date *p_d);

void main(void)
{
   byte mem_adr;
   byte n;

   struct Time t_base={0x23, 0x59, 0x00}, t;
   struct Date d_base={0x00, 0x02, 0x28, 0x07}, d;

   lcd_init();
   i2c_master_setup();

   set_up_clock(0x10);  // sqwe enabled, 1 Hz output
   write_clock(&t_base, &d_base);

   while(1)
   {
      read_clock(&t, &d);
      lcd_init();
      printf(lcd_char, "%x/%x/%x %x:%x:%x",
                              d.yr, d.mo, d.da, t.hr, t.mi, t.se);
      lcd_clr_line(1);
      printf(lcd_char, "%x", d.weekday);
      delay_ms(1000);
   }
}

void set_up_clock(int control_reg)
{
   i2c_master_start();
   i2c_master_out_byte(0xd0);
   i2c_master_out_byte(0x07);  // control register address
   i2c_master_out_byte(control_reg);
   i2c_master_stop();
}

void write_clock(struct Time *p_t, struct Date *p_d)
{
   i2c_master_start();
   i2c_master_out_byte(0xd0);	  // address
   i2c_master_out_byte(0x00);     // first address
   i2c_master_out_byte(p_t->se);
   i2c_master_out_byte(p_t->mi);
   i2c_master_out_byte(p_t->hr);  // 24 hour format - 0x40 for 12 hour time
   i2c_master_out_byte(p_d->weekday);
   i2c_master_out_byte(p_d->da);
   i2c_master_out_byte(p_d->mo);
   i2c_master_out_byte(p_d->yr);
   i2c_master_stop();
}

void read_clock(struct Time *p_t, struct Date *p_d)
{
   i2c_master_start();
   i2c_master_out_byte(0xd0);   // 1101 000 0
   i2c_master_out_byte(0x00);  // first address
   i2c_master_stop();

   i2c_master_repeated_start();
   i2c_master_out_byte(0xd1);
   p_t->se = i2c_master_in_byte(TRUE) & 0x7f;
   p_t->mi = i2c_master_in_byte(TRUE);
   p_t->hr = i2c_master_in_byte(TRUE) & 0x3f;

   p_d->weekday = i2c_master_in_byte(TRUE) & 0x07;
   p_d->da = i2c_master_in_byte(TRUE) & 0x3f;
   p_d->mo = i2c_master_in_byte(TRUE) & 0x3f;
   p_d->yr = i2c_master_in_byte(FALSE);
   // no ack prior to stop
   i2c_master_stop();
}

#include <lcd_out.c>
#include <i2c_mstr.c>




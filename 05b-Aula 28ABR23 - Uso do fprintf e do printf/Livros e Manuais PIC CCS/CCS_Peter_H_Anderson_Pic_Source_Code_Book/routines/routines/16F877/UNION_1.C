// Program UNION_1.C
//
// Illustrates the use of a union to such that a long and two bytes share
// the same memory space.
//
// Peter H. Anderson, Baltimore, MD, Jan, '01

#case

#device PIC16F877 *=16 ICD=TRUE

#include <defs_877.h>
#include <lcd_out.h>

#define TRUE !0
#define FALSE 0

struct TWO_BYTES
{
	byte l;
	byte h;
};

union LONG
{
	unsigned long w;
	struct TWO_BYTES b;
};

unsigned long make_ad_meas(void);

void main(void)
{

   union LONG adval;
   lcd_init();
   adval.b.h = 0x01;
   adval.b.l = 0x80;
   printf(lcd_char, "%lx", adval.w);

   adval.w = make_ad_meas();
   lcd_clr_line(1);
   printf(lcd_char, "%lx", adval.w);
   while(1)		/* loop continually */	;
}

unsigned long make_ad_meas(void)
{
   union LONG x;
   x.b.h = 0x02;
   x.b.l = 0x80;
   return(x.w);
}

#include <lcd_out.c>




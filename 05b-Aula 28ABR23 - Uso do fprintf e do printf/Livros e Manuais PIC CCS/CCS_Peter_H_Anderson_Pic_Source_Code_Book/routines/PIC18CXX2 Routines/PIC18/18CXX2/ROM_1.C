// ROM_1.C
//
// Illustrates how to initialize memory locations byte by byte using the #rom
// directive.

// copyright, Peter H Anderson, Baltimore, MD, Jan, '02

#case

#device PIC18C452

#include <defs_18c.h>
#include <delay.h>
#include <ser_18c.h>


#define FALSE 0
#define TRUE !0

struct TABLE
{
	byte ptr_u;
	byte ptr_h;
	byte ptr_l;
	byte lat;
};

void get_str_mem(byte str_num, char *s);
void out_str(char *s);

void TBL_to_RAM(struct TABLE *p);
void RAM_to_TBL(struct TABLE *p);
void decrement_table_ptr(struct TABLE *p);

void main(void)
{
   byte n;
   char s[80];

   ser_init();
   while(1)
   {
       printf(ser_char, "\n\r......................\n\r");
       get_str_mem(0, s);
       out_str(s);
       ser_newline();
       delay_ms(500);
   }
}

void get_str_mem(byte str_num, char *s)
{

   byte n, dat;

   struct TABLE program_table_save, compiler_table_save;

	program_table_save.ptr_u = 0x00;
	program_table_save.ptr_h = 0x10;
	program_table_save.ptr_l = 0x00;

   decrement_table_ptr(&program_table_save);


	TBL_to_RAM(&compiler_table_save);	// save the compiler values
	RAM_to_TBL(&program_table_save);	// fetch the program values

	n = 0;
	while(1)
	{

#asm
      TBLRD
#endasm
      dat = TABLAT;
      s[n] = dat;
      if (s[n] == '\0')
      {
			 break;
		}
		++n;
		if (n==80)
		{
			 break;
		}
   }
   RAM_to_TBL(&compiler_table_save);	// restore the compiler values
}


void out_str(char *s)
{
	byte n;

	n=0;
	while(s[n])
	{
		ser_char(s[n]);
		++n;
      if (n==80)
      {
         break;
      }
	}
}


void TBL_to_RAM(struct TABLE *p)
{
	p->ptr_u = TBLPTRU;
	p->ptr_h = TBLPTRH;
    p->ptr_l = TBLPTRL;
    p->lat = TABLAT;
}

void RAM_to_TBL(struct TABLE *p)
{
    TBLPTRU = p->ptr_u;
    TBLPTRH = p->ptr_h;
    TBLPTRL = p->ptr_l;
    TABLAT = p->lat;
}

void decrement_table_ptr(struct TABLE *p)
{
	--(p->ptr_l);
	if (p->ptr_l == 0xff)
	{
		--(p->ptr_h);
		if (p->ptr_h == 0xff)
		{
			--(p->ptr_u);
         p->ptr_u = p->ptr_u & 0x0f;
		}
	}
}


#rom 0x1000 = {'M'}
#rom 0x1001 = {'o'}
#rom 0x1002 = {'r'}
#rom 0x1003 = {'g'}
#rom 0x1004 = {'a'}
#rom 0x1005 = {'n'}
#rom 0x1006 = {' '}
#rom 0x1007 = {'S'}
#rom 0x1008 = {'t'}
#rom 0x1009 = {'a'}
#rom 0x100a = {'t'}
#rom 0x100b = {'e'}
#rom 0x100c = {' '}
#rom 0x100d = {'U'}
#rom 0x100e = {'n'}
#rom 0x100f = {'i'}
#rom 0x1010 = {'v'}
#rom 0x1011 = {'e'}
#rom 0x1012 = {'r'}
#rom 0x1013 = {'s'}
#rom 0x1014 = {'i'}
#rom 0x1015 = {'t'}
#rom 0x1016 = {'y'}
#rom 0x1017 = {'\0'}

#include <delay.c>
#include <ser_18c.c>


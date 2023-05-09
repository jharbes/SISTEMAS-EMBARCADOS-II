// Program MEM_SAVE.C
//
// Illustrates how to save a quantity to and fetch a quantity from program memory..
//
// Saves a float and a struct TM to EEPROM and then fetches them and displays
// on the terminal.
//
// Note that a byte pointer which points to the beginning of the quantity and the number
// of bytes is passed to each function.
//
// copyright, Peter H. Anderson, Baltimore, MD, Jan, '02

#case

#device PIC18C452

#include <defs_18c.h>
#include <delay.h>
#include <ser_18c.h>

struct TABLE
{
	byte ptr_u;
	byte ptr_h;
	byte ptr_l;
	byte lat;
};

struct LONG24
{
	byte u;
	byte h;
	byte l;
};

struct TM
{
	byte hr;
	byte mi;
	byte se;
};


void save_to_memory(struct LONG24 *p_adr, byte *p_dat, byte num_bytes);
void read_from_memory(struct LONG24 *p_adr, byte *p_dat, byte num_bytes);

void TBL_to_RAM(struct TABLE *p);
void RAM_to_TBL(struct TABLE *p);
void decrement_table_ptr(struct TABLE *p);

void main(void)
{
   float float_1 = 1.2e-12, float_2;
   struct TM t1, t2;
   struct LONG24 adr;

   byte *ptr;

   ser_init();

   printf(ser_char, "\r\n......................\r\n");

   adr.u = 0x00;  adr.h = 0x20;  adr.l = 0x00;	// 0x002000

   ptr = (byte *) &float_1;		// ptr points to first byte of float_1
   save_to_memory(&adr,  ptr, sizeof(float));	// save float_1

   t1.hr = 12;	t1.mi = 45; 	t1.se = 33;

   adr.u = 0x00;  adr.h = 0x20;  adr.l = 0x04;	// 0x002004
   ptr = (byte *) &t1;
   save_to_memory(&adr, ptr, sizeof(struct TM));	// save t1


   adr.u = 0x00;  adr.h = 0x20;  adr.l = 0x00;	// 0x002000
   ptr = (byte *) &float_2;
   read_from_memory(&adr, ptr, sizeof(float));


   adr.u = 0x00;  adr.h = 0x20;  adr.l = 0x04;	// 0x002004
   ptr = (byte *) &t2;
   read_from_memory(&adr, ptr, sizeof(struct TM));


   printf(ser_char, "float = %1.3e\r\n", float_2);	// print the float

   printf(ser_char, "t2 = ");
   ser_dec_byte(t2.hr, 2);
   ser_char(':');
   ser_dec_byte(t2.mi, 2);
   ser_char(':');
   ser_dec_byte(t2.se, 2);
   ser_newline();

   while(1)
   {
#asm
   CLRWDT
#endasm
   }
}

void save_to_memory(struct LONG24 *p_adr, byte *p_dat, byte num_bytes)
{
   byte n;
   struct TABLE compiler_table_save, program_table_save;

   program_table_save.ptr_u = p_adr->u;
   program_table_save.ptr_h = p_adr->h;
   program_table_save.ptr_l = p_adr->l;

   decrement_table_ptr(&program_table_save);

   TBL_to_RAM(&compiler_table_save);	// save compiler values
   RAM_to_TBL(&program_table_save);

   for (n=0; n<num_bytes; n++)
   {
	   TABLAT = *p_dat;
#asm
	   TBLWT
#endasm
	   ++p_dat;
   }

   RAM_to_TBL(&compiler_table_save);	// restore compiler values
}

void read_from_memory(struct LONG24 *p_adr, byte *p_dat, byte num_bytes)
{
   byte n;
   struct TABLE compiler_table_save, program_table_save;

   program_table_save.ptr_u = p_adr->u;
   program_table_save.ptr_h = p_adr->h;
   program_table_save.ptr_l = p_adr->l;

   decrement_table_ptr(&program_table_save);

   TBL_to_RAM(&compiler_table_save);	// save compiler values
   RAM_to_TBL(&program_table_save);

   for (n=0; n<num_bytes; n++)
   {
#asm
       TBLRD
#endasm

	   *p_dat = TABLAT;
	   ++p_dat;
   }
   RAM_to_TBL(&compiler_table_save);	// restore compiler values
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


#include <delay.c>
#include <ser_18c.c>


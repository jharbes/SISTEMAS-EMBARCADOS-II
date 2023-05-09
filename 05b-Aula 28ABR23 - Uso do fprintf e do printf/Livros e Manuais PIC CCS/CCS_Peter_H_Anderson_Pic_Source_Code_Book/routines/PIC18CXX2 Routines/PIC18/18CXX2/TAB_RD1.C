// Program TAB_RD1.C
//
// Raeds and displays the first 256 byte of program memory to the terminal.
//
// Then displays configuration registers beginning at 0x030000
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

void display_program_memory(long page, long num_bytes);

void TBL_to_RAM(struct TABLE *p);
void RAM_to_TBL(struct TABLE *p);
void decrement_table_ptr(struct TABLE *p);



void main(void)
{

	byte x, dat;
	long n;
	struct TABLE compiler_table_save, program_table_save;

	ser_init();

	printf(ser_char, ".............\n\r");
	display_program_memory(0x0000, 256);

	printf(ser_char, ".............\n\r");
	display_program_memory(0x0300, 10);

	while(1)	/* loop */  ;
}


void display_program_memory(long page, long num_bytes)
{
   byte dat;
	long n;
   struct TABLE program_table_save, compiler_table_save;

	program_table_save.ptr_u = (byte) (page >> 8);
	program_table_save.ptr_h = (byte) page;
	program_table_save.ptr_l = 0x00;

    decrement_table_ptr(&program_table_save);

	for(n=0; n<num_bytes; n++)
	{
	   if (n%16 == 0)
	   {
		   printf(ser_char, "\n\r");
	   }
	   TBL_to_RAM(&compiler_table_save);	// save the compiler values
	   RAM_to_TBL(&program_table_save);	// fetch the program values

#asm
       TBLRD
#endasm
       dat = TABLAT;
       TBL_to_RAM(&program_table_save);	// save these vals
	   RAM_to_TBL(&compiler_table_save);	// fetch the compiler values
	   printf(ser_char, "%2x ", dat);
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


#include <delay.c>
#include <ser_18c.c>

// Program TAB_WT1.C
//
// Illustrates writing to and reading from program memory using TBLWT and TBLRD.
//
// Program checks program memory location 0x001000 and if blank (0xff), writes
// copyright meassage beginning at location 0x001000.
//
// Reads and displays message on terminal.
//
// copyright, Peter H. Anderson, Baltimore, MD, Jan, '02

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

byte first_time(void);					// TRUE if location is at 0xff
void write_copyright_message(void);
void display_copyright_message(void);

void TBL_to_RAM(struct TABLE *p);
void RAM_to_TBL(struct TABLE *p);
void decrement_table_ptr(struct TABLE *p);

void main(void)
{

	byte x, dat;
	long n;

	ser_init();

    printf(ser_char, "\n\r");

	if(first_time() == TRUE)
	{
	   write_copyright_message();
    }

    display_copyright_message();

    while(1)   /* loop */  ;		// to keep emulator from stalling
}

byte first_time(void)
{
	struct TABLE compiler_table_save, program_table_save;
    byte dat;

	program_table_save.ptr_u = 0x00;
	program_table_save.ptr_h = 0x10;
	program_table_save.ptr_l = 0x00;

    TBL_to_RAM(&compiler_table_save);		// save the compiler values

	decrement_table_ptr(&program_table_save);
	RAM_to_TBL(&program_table_save);	// fetch the program values into TBL pointer

#asm
    TBLRD
#endasm
    dat = TABLAT;

	RAM_to_TBL(&compiler_table_save);	// fetch the compiler values to TBL pointer
	// printf(ser_char, "%2x\n\r", dat);

	if(dat == 0xff)
    {
      return(TRUE);
    }
    else
    {
      return(FALSE);
    }
}

void write_copyright_message(void)
{
	struct TABLE compiler_table_save, program_table_save;
    char s[80] = {"copyright, Peter H. Anderson, Baltimore, MD\n\r"};
	byte s_length, n;

	program_table_save.ptr_u = 0x00;
	program_table_save.ptr_h = 0x10;
	program_table_save.ptr_l = 0x00;

	TBL_to_RAM(&compiler_table_save);		// save the compiler values

	decrement_table_ptr(&program_table_save);
	RAM_to_TBL(&program_table_save);	// fetch the program values into TBL pointer


	for(n=0; ; n++)
	{
		TABLAT = s[n];
#asm
        TBLWT
#endasm
        if (s[n] == '\0')
        {
			break;
		}
	}

	RAM_to_TBL(&compiler_table_save);	// restore compiler values
}

void display_copyright_message(void)
{
	struct TABLE compiler_table_save, program_table_save;

	byte dat, n;

	program_table_save.ptr_u = 0x00;
	program_table_save.ptr_h = 0x10;
	program_table_save.ptr_l = 0x00;

	decrement_table_ptr(&program_table_save);

	for(n=0; ; n++)
	{

	   TBL_to_RAM(&compiler_table_save);	// save the compiler values
	   RAM_to_TBL(&program_table_save);	// fetch the program values

#asm
       TBLRD
#endasm
       dat = TABLAT;
       TBL_to_RAM(&program_table_save);	// save these vals
	   RAM_to_TBL(&compiler_table_save);	// fetch the compiler values
	   if (dat == '\0')
	   {
		   break;
	   }
	   printf(ser_char, "%c", dat);
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

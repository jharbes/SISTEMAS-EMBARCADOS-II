// Program ARRY_TST.C
//
// The intent of this program is to test that the CCS PCH compiler properly
// handles arrays of longs in functions.
//
// The values of an array of longs are fetched, one is added to each value
// and the array is displayed.
//
// copyright, Peter H. Anderson, Baltimore, Jan, '02

#case
#device PIC18C452

#include <defs_18c.h>
#include <delay.h>
#include <ser_18c.h>

#define FALSE 0
#define TRUE !0


void fetch_array(unsigned long *a);
void add_one(unsigned long *a);
void print_array(unsigned long *a);

void main(void)
{
	unsigned long a[20];

	ser_init();

	fetch_array(a);
    add_one(a);
	print_array(a);

	while(1)  /* loop */     ;
}

void fetch_array(unsigned long *a)
{
	const unsigned long c[20] = {0x1234, 0x2345, 0x3456, 0x4567, 0x5678,
	 			     			 0x1235, 0x2346, 0x3457, 0x4568, 0x5679,
	                             0x1236, 0x2347, 0x3458, 0x4569, 0x567a,
	                             0x1237, 0x2348, 0x3459, 0x456a, 0x567b};

	byte n;

	for (n=0; n<20; n++)
	{
		a[n] = c[n];
	}
}

void add_one(unsigned long *a)
{
	byte n;

	for (n=0; n<20; n++)
	{
		a[n] = a[n] + 1;
	}
}

void print_array(unsigned long *a)
{
	byte n;

	printf(ser_char, "****************\r\n");

	for (n=0; n<20; n++)
	{
		printf(ser_char, "%2x%2x\r\n", (byte) (a[n] >> 8), (byte) (a[n] & 0xff));
	}
}

#include <delay.c>
#include <ser_18c.c>

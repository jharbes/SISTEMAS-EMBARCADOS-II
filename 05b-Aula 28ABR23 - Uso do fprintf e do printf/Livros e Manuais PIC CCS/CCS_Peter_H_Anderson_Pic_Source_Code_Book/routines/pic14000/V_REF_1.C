// V_REF_1.C - PIC16F877 (for PIC14000)
//
//
// copyright, Peter H Anderson, Elmore, VT, June, '02

#case

#device PIC16F877 *=16 ICD=TRUE

#include <defs_877.h>
#include <delay.h>
#include <ser_14.h>

#define MAKE_LONG(h, l)  (((unsigned long) (h)) << 8) | (l)

#define TRUE !0
#define FALSE 0

#define TxData 2	// PORTC, Bit 2
#define INV


byte set_vref(byte channel, float vref_val);


void main(void)
{
	byte x, n;
	float v;

    ser_init();

    for (n=0; n<100; n++)
    {
		v = 0.55 + 0.3 * (float) n / 100.0;
		x = set_vref(0, v);

		printf(ser_char, "%f  %2x", v, x);
		delay_ms(5000);
		ser_new_line();
    }
}


byte set_vref(byte channel, float vref_val)
{

	byte x;

	if (vref_val > 0.55)
	{
		x = ((byte) ((vref_val - 0.55)/0.05) + 0x0a) * 8;
	}
	else if (vref_val < 0.45)
	{
		x = ((byte) ((0.45 - vref_val) / 0.05) + 0x0a) * 8 + 0x80;
	}

	else if (vref_val >=0.5)
	{
		x = ((byte) ((vref_val - 0.5) / 0.005)) * 8;
	}

	else
	{
		x = ((byte) ((0.5 - vref_val) / 0.005)) * 8 + 0x80;
	}

	return(x);
}

#include <delay.c>
#include <ser_14.c>



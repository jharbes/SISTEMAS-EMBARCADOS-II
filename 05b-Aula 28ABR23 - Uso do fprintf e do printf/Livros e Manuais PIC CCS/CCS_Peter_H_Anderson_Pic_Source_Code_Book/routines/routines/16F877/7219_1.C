// 7219_1.C,
//
// Illustrates the use of a MAX7219 8-digit LED driver to control four
// common cathode 7-segment LEDs.
//
// Sets up MAX7219 for code B decoding (4-bit), blanks all digits and displays 0 - 15
// on least significant LED display.
//
// Displays 990 through 1090 with leading zero suppression.
//
//	PIC16F877								MAX7219
//
// RC5/SDO (term 24) ------------------->DIN (term 1)
// RC4/SDI (term 23) <------
// RC3/SCK (term 18) ------------------->CLK (term 13)
// RB2/CS (term 35) -------------------->/CS (term 12)
//
// copyright, Peter H. Anderson, Baltimore, MD, Jan, '01

#case

#device PIC16F877 *=16 ICD=TRUE

#include <defs_877.h>
#include <lcd_out.h>

#define TRUE !0
#define FALSE 0

void _7219_dec_display_unsigned(unsigned long v, byte zero_suppression);
void _7219_blank_all(void);
void _7219_setup(void);
void _7219_shutdown(void);

void _7219_display_4(byte *patts);
void _7219_digit_display(byte digit_num, byte d);

void _7219_out(unsigned long d);

#define NO_OP 0x0000
#define DECODE_MODE 0x0900
#define INTENSITY 0x0a00
#define SCAN_LIMIT 0x0b00
#define SHUT_DOWN 0x0c00
#define DISP_TEST 0x0f00

#define BLANK 0x0f
#define MINUS 0x0a	// Code B for a minus sign

void main(void)
{
   int n;
   unsigned long v;

   while(1)
   {
      _7219_setup();      // turn on, code B decoding, medium intensity, 4 digits
      _7219_blank_all();
      for (n=0; n<16; n++)
      {
         _7219_digit_display(0, n);
         // display each of the characters on least signif display
         delay_ms(1000);
      }

      for (n=0; n<10; n++)
      {
         v = 995 + n;
         _7219_dec_display_unsigned(v, 1);   // leading zero suppression on
         delay_ms(1000);
      }
   }
}


void _7219_dec_display_unsigned(unsigned long v, byte zero_suppression)
// leading zero suppression
{
   byte d, digits[4];

   d = v/1000;    // display the number of thousands
   if( (d) || (!zero_suppression) )
   // if the digit is not zero OR if there is no zero suppression
   {
      digits[3] = d;
      zero_suppression = 0;		// no zero suppression
   }
   else  // d is zero and zero suppression
   {
      digits[3] = BLANK;
   }
   v = v % 1000;

   d = v / 100;   // display the number of hundreds
   if( (d) || (!zero_suppression))
   {
      digits[2] = d;
      zero_suppression = 0;
   }
   else
   {
      digits[2] = BLANK;
   }

   v = v % 100;
   d = v / 10;    // tens
   if( (d) || (!zero_suppression))
   {
      digits[1] = d;
   }
   else
   {
      digits[1] = BLANK;
   }

   v = v % 10;
   d = v;      // units
   digits[0] = d;
   _7219_display_4(digits);
}

void _7219_blank_all(void)
{
   byte n, digits[4];
   for(n=0; n<4; n++)
   {
      digits[n]=BLANK;
   }
   _7219_display_4(digits);
}

void _7219_display_4(int *patts)   // display content of patts
{
   byte n;
   for(n=0; n<4; n++)
   {
      _7219_digit_display(n, patts[n]);
   }
}

void _7219_digit_display(byte digit_num, byte d)
      // display d on display digit_num
{
   long v;
   v = ((long) (digit_num + 1) << 8) | d;	// display number is in high byte
   _7219_out(v);
}


void _7219_setup(void)
{
   sspen = 0;
   sspen = 1;
   sspm3 = 0; sspm2 = 0; sspm1 = 1; sspm0 = 0;	// Configure as SPI Master, fosc / 64
   ckp = 0; 							// idle state for clock is zero
   stat_cke = 1;						// data transmitted on rising edge
   stat_smp = 1; 						// not really necessary in this application

   portc3 = 0;
   trisc3 = 0;	// SCK as output 0

   trisc4 = 1;	// SDI as input
   trisc5 = 0;	// SDO as output

   rb2 = 1;
   trisb2 = 0;	// be sure CS is high

   _7219_out(DISP_TEST | 0x00);  // normal
   _7219_out(SHUT_DOWN | 0x01);  // take it out of low power mode
   _7219_out(SCAN_LIMIT | 0x04);  // 4 digits
   _7219_out(DECODE_MODE | 0xff);  //code B decode
   _7219_out(INTENSITY | 0x08);  // medium intensity
}

void _7219_shutdown(void)
{
   _7219_out(SHUT_DOWN | 0x00);  // turn it off
}

void _7219_out(unsigned long d)
{
   byte dummy;

   rb2 = 1;
   trisb2 = 0;

   rb2 = 0;		// bring CS low
   delay_10us(10);

   SSPBUF = (byte) (d >> 8);
   while(!stat_bf)   /* loop */    ;
   dummy = SSPBUF;

   SSPBUF = (byte) (d);
   while(!stat_bf)   /* loop */           ;
   dummy = SSPBUF;

   rb2 = 1;		// CS high
}

#include <lcd_out.c>


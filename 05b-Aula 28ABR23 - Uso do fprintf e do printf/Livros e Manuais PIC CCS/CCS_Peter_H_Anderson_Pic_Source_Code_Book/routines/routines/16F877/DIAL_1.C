// Program DIAL_1.C
//
// Dials the telephone number 1-800-555-1212 and sends data T_F using
// 200 ms zips of nominally 500 Hz.  The send data sequence is repeated
// three times and the processor then hangs up.
//
// LED (simulating dial pulse relay) on PORTD.4.  Speaker through 47 uFd on
// PORTD.0.  Pushbutton on input PORTB.0.
//
// copyright, Peter H. Anderson, Baltimore, MD, Dec, '00
//

#case

#device PIC16F877 *=16 ICD=TRUE

#include <defs_877.h>
#include <lcd_out.h>

void dial_tel_num(void);
void dial_digit(byte num);
void send_quan(byte q);
void zips(byte x);
void zip(byte duration);

void main(void)
{

   byte T_F = 103, n;

   pspmode = 0;
   portd4 = 0;
   trisd4 = 0;             // dial pulse relay

   trisd0 = 0;             // speaker

   trisb0 = 1;             // pushbutton is an input
   not_rbpu = 0;           // enable internal pullups

   while(1)
   {

      while(rb0)           // loop until pushbutton depressed
      {
      }
      portd4 = 1;          // go off hook
      delay_ms(1000);      // wait for dial tone

      dial_tel_num();

      delay_ms(1000);      // wait for answer

      for (n=0; n<3; n++)  // send the quantity T_F three time
      {
         send_quan(T_F);
         delay_ms(1500);
      }

      portd4 = 0;          // back on-hook
   }

}


void dial_tel_num(void)
{
    const byte tel_num[20] = {1, 8, 0, 0, 5, 5, 5, 1, 2, 1, 2, 0x0f};
    byte n;

    for (n=0; n<20; n++)      // up to 20 digits
    {
        if (tel_num[n] == 0x0f)  // if no more digits
        {
            break;
        }

        else
        {
            dial_digit(tel_num[n]);
        }
        delay_ms(500);  // inter digit delay
    }
}

void dial_digit(byte num)
{
    byte n;
    for (n=0; n<num; n++)
    {
       portd4 = 0;   // 63 percent break at 10 pulses per second
       delay_ms(63);
       portd4 = 1;
       delay_ms(37);
    }
}

void send_quan(byte q)
{
    byte x;
    if (q > 99)         // if three digits
    {
       x = q/100;
       zips(x);         // sned the hundreds
       delay_ms(500);
       q = q % 100;     // strip off the remainder
    }
    x = q / 10;
    zips(x);            // send the tens
    delay_ms(500);
    x = q % 10;
    zips(x);            // units
}

void zips(byte x)
{
    byte n;
    if (x == 0)
    {
        x = 10;
    }
    for (n=0; n<x; n++)
    {
        zip(200);
        delay_ms(200);
    }
}

void zip(byte duration)
{
   byte n;
   trisd0 = 0;
   for (n= duration/2; n>0; n--) // duration/2 * 2 ms
   {
       portd0 = 1;
       delay_10us(100); // 1 ms
       portd0 = 0;
       delay_10us(100);
   }
}

#include <lcd_out.c>

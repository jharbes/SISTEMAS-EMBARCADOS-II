// delay.c
// copyright, Peter H. Anderson, Elmore, VT, July, '01

void delay_10us(byte t)
{
#asm
DELAY_10US_1:
      CLRWDT
      NOP
      NOP
      NOP
      NOP
      NOP
      NOP
      DECFSZ t, F
      GOTO DELAY_10US_1
#endasm
}

void delay_ms(long t)   // delays t millisecs
{
   do
   {
     delay_10us(100);
   } while(--t);
}


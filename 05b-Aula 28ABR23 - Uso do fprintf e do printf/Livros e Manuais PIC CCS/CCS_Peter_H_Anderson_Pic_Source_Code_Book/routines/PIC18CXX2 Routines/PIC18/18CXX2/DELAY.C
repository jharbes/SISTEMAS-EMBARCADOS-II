// delay.c
// copyright, Peter H. Anderson, Baltimore, MD, Jan, '02

void delay_10us(byte t)
{
#asm
      GOTO DELAY_10US_2
DELAY_10US_1:
      NOP      
      NOP
      NOP
      NOP
      NOP
      NOP
DELAY_10US_2:      
      CLRWDT   
      DECFSZ t, F
      GOTO DELAY_10US_1
#endasm
}

void delay_ms(long t)   // delays t millisecs
{
   do
   {
     delay_10us(99);    // not 100 to compensate for overhead
   } while(--t);
}


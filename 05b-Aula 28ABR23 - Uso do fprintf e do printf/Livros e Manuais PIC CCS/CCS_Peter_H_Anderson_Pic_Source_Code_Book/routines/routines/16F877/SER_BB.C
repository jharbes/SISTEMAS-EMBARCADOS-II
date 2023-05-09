// SER_BB.C
//
// Illustrates RS232 communication using bit-bang.
//
// Program listens for a string terminated with 0x0d from PC Com Port and
// then sends the string to the PC Com Port.
//
//  PC Com Port				   PIC16F877
//
// Tx (term 3) ------- 22K ----------> RA1 (RxData)
// Rx (term 2) <---------------------- RA0 (TxData)
//
// copyright, Peter H Anderson, Baltimore, MD, June, '01

#case

#device PIC16F877 *=16 ICD=TRUE
#include <a:\defs_877.h>
#include <a:\delay.c>

#define FALSE 0
#define TRUE !0

void ser_bb_init(void);
void ser_bb_char(byte ch);
void ser_bb_new_line(void);

void ser_bb_out_str(byte *s);

byte ser_bb_get_str_1(byte *p_chars, long t_wait_1, long t_wait_2, byte term_char);
byte ser_bb_get_str_2(byte *p_chars, long t_wait_1, long t_wait_2, byte num_chars);
byte ser_bb_get_ch(long t_wait);

#define INV  // inverted bit-bang

#define TxData 0 // PortA.0
#define RxData 1 // PortA.1

void main(void)
{
    byte s[20];
    pcfg3 = 0;  pcfg2 = 1;   pcfg1 = 1;  // configure A/D as 0/0
    ser_bb_init();
    while(1)
    {
       if (ser_bb_get_str_1(s, 10000, 1000, 0x0d))
       {
          ser_bb_out_str(s);
          ser_bb_new_line();
	   }
	   else
	   {
		   ser_bb_char('!');	// to show something is going on
	   }
    }
 }
}


void ser_bb_init(void) // sets TxData in idle state
{
#ifdef INV    // idle is TTL logic zero
#asm
   BCF STATUS, RP0
   BCF PORTA, TxData
   BSF STATUS, RP0
   BCF TRISA, TxData // TxData is an oputput
   BSF TRISA, RxData // RxData is an inout
   BCF STATUS, RP0
#endasm

#else
#asm
   BSF STATUS, RP0
   BCF PORTA, TxData // idle state is TTL logic one
   BSF STATUS, RP0
   BCF TRISA, TxData
   BSF TRISA, RxData
   BCF STATUS, RP0
#endasm
#endif
}

void ser_bb_char(byte ch) // serial output 9600 baud
{
   byte n, dly;
     // start bit + 8 data bits
#ifdef INV    // idle is TTL logic zero
#asm
   BCF STATUS, RP0
   MOVLW 9
   MOVWF n
   BCF STATUS, C

SER_BB_CHAR_1:

   BTFSS STATUS, C		// 4 ~
   BSF PORTA, TxData
   BTFSC STATUS, C
   BCF PORTA, TxData
   MOVLW 31				// 1 ~
   MOVWF dly			// 1 ~

SER_BB_CHAR_2:
   DECFSZ dly, F			// 31 * 3 ~
   GOTO SER_BB_CHAR_2		// 1 ~
   RRF ch, F			// 1 ~
   DECFSZ n, F			// 1 ~
   GOTO SER_BB_CHAR_1	// next bit	// 2 ~

   BCF PORTA, TxData	// idel between characters
   CLRWDT
   MOVLW 96
   MOVWF dly

SER_BB_CHAR_3:
   DECFSZ dly, F
   GOTO SER_BB_CHAR_3
   CLRWDT
#endasm

#else  // idle is TTL logic one
#asm
   BCF STATUS, RP0
   MOVLW 9
   MOVWF n
   BCF STATUS, C

SER_BB_CHAR_1:

   BTFSS STATUS, C
   BCF PORTA, TxData
   BTFSC STATUS, C
   BSF PORTA, TxData
   MOVLW 32
   MOVWF dly

SER_BB_CHAR_2:
   DECFSZ dly, F
   GOTO SER_BB_CHAR_2
   RRF ch, F
   DECFSZ n, F
   GOTO SER_BB_CHAR_1

   BSF PORTA, TxData // back to idle state
   CLRWDT
   MOVLW 96
   MOVWF dly

SER_BB_CHAR_3:
   DECFSZ dly, F
   GOTO SER_BB_CHAR_3
   CLRWDT
#endasm
#endif

}

void ser_bb_new_line(void) // outputs 0x0d, 0x0a
{
   ser_bb_char(0x0d);
   delay_ms(10); // give the PIC-n-LCD time to perform the
   ser_bb_char(0x0a); // new line function
   delay_ms(10);
}

void ser_bb_out_str(byte *s)
{
   while(*s)
   {
      ser_bb_char(*s);
      ++s;
   }
}

byte ser_bb_get_str_1(byte *p_chars, long t_wait_1, long t_wait_2, byte term_char)
// assumes p_chars is dimensioned as 20 bytes
{
   byte n=0, ch;

   if((ch = ser_bb_get_ch(t_wait_1)) == 0xff)
   {
     p_chars[0]=0; // return null
     return(0);
   }
   else if (ch == term_char)
   {
     p_chars[0]=0; // null terminate the string
     return(0);
   }
   else
   {
      p_chars[0]=ch;
   }

   for(n=1; n<19; n++)
   {
      if((ch = ser_bb_get_ch(t_wait_2)) == 0xff)
      {
         p_chars[n] = 0;
         return(n);
      }
      else if (ch == term_char)
      {
        p_chars[n]=0; // null terminate the string
        return(n);
      }
      else
      {
         p_chars[n]=ch;
      }
   }
   p_chars[n] = 0; // null terminate
}

byte ser_bb_get_str_2(byte *p_chars, long t_wait_1, long t_wait_2, byte num_chars)
{
   byte n, ch;

   if((ch = ser_bb_get_ch(t_wait_1)) == 0xff)
   {
      p_chars[0]=0; // return null
      return(0);
   }
   else
   {
      p_chars=ch;
   }

   for(n=1; n<num_chars; n++)
   {

      if((ch = ser_bb_get_ch(t_wait_2)) == 0xff)
      {
        p_chars[n]=0; // return null
        return(n);
      }

      else
      {
         p_chars[n]=ch;
      }
   }
   p_chars[n]=0; // null terminate - this is the normal termination
   return(n);
}

byte ser_bb_get_ch(long t_wait)
// returns character.  If no char received within t_wait ms, returns 0xff.
#ifdef INV
{
   byte one_ms_loop,  ser_loop, ser_data, ser_time;
   do
   {
        one_ms_loop = 100; // 100 times 10 usecs
#asm

SCAN_1:
    CLRWDT
    NOP
    NOP
    NOP
    NOP
    BTFSC PORTA, RxData // check serial in - for inverted data
    GOTO  SERIN_1  // if start bit
    DECFSZ  one_ms_loop, F
    GOTO SCAN_1
#endasm
   }while(--t_wait);
   return(0xff);

#asm
SERIN_1:

    MOVLW 8
    MOVWF ser_loop
    CLRF ser_data

    MOVLW 51          // delay for 1.5 bits
    MOVWF ser_time  // 3 + 51 * 3 = 156 usecs for 9600

SERIN_2:
    DECFSZ ser_time, F
    GOTO SERIN_2

SERIN_3:
    BTFSS PORTA, RxData
    BSF STATUS, C  // reverse these for non inverted
    BTFSC PORTA, RxData
    BCF STATUS, C

    RRF ser_data, F

    MOVLW 23        // one bit delay
    MOVWF ser_time  // 10 + 23 * 4 =  102 usecs

SERIN_4:
    CLRWDT
    DECFSZ ser_time, F
    GOTO SERIN_4

    DECFSZ ser_loop, F // done?
    GOTO SERIN_3 // get next bit

    MOVLW 10
    MOVWF ser_time  // wait for at least 1/2 bit

SERIN_5:
    CLRWDT
    DECFSZ ser_time, F
    GOTO SERIN_5
#endasm
   return(ser_data);
}

#else
{
   byte one_ms_loop,  ser_loop, ser_data, ser_time;
   do
   {
        one_ms_loop = 100; // 100 times 10 usecs
#asm

SCAN_1:
    CLRWDT
    NOP
    NOP
    NOP
    NOP
    BTFSS PORTA, RxData
    GOTO  SERIN_1  // if start bit
    DECFSZ  one_ms_loop, F
    GOTO SCAN_1
#endasm
   }while(--t_wait);
   return(0xff);

#asm
SERIN_1:

    MOVLW 8
    MOVWF ser_loop
    CLRF ser_data

    MOVLW 51          // delay for 1.5 bits
    MOVWF ser_time  // 3 + 51 * 3 = 156 usecs for 9600

SERIN_2:
    DECFSZ ser_time, F
    GOTO SERIN_2

SERIN_3:
    BTFSS PORTA, RxData
    BCF STATUS, C
    BTFSC PORTA, RxData
    BSF STATUS, C

    RRF ser_data, F

    MOVLW 23        // one bit delay
    MOVWF ser_time  // 10 + 23 * 4 =  102 usecs

SERIN_4:
    CLRWDT
    DECFSZ ser_time, F
    GOTO SERIN_4

    DECFSZ ser_loop, F // done?
    GOTO SERIN_3 // get next bit

    MOVLW 10
    MOVWF ser_time  // wait for at least 1/2 bit

SERIN_5:
    CLRWDT
    DECFSZ ser_time, F
    GOTO SERIN_5
#endasm
   return(ser_data);
}
#endif


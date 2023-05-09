// SER_14.C
//
//
// copyright, Peter H Anderson, Elmore, VT, June, '02

void ser_init(void) // sets TxData in idle state
{
#ifdef INV    // idle is TTL logic zero
#asm
   BCF STATUS, RP0
   BCF PORTC, TxData
   BSF STATUS, RP0
   BCF TRISC, TxData // TxData is an oputput
   BCF STATUS, RP0
#endasm

#else
#asm
   BCF STATUS, RP0
   BCF PORTC, TxData // idle state is TTL logic one
   BSF STATUS, RP0
   BCF TRISC, TxData
   BCF STATUS, RP0
#endasm
#endif

   ser_char(0x0c); // unique to PIC-n-LCD
   delay_ms(50);
}

void ser_char(byte ch) // serial output 9600 baud
{
   byte n, dly, gie_flag;
     // start bit + 8 data bits
   if(gie)		// if gie is enabled, turn it off
   {
	   gie_flag = TRUE;
	   while(gie)
	   {
		   gie = FALSE;
	   }
   }

   else
   {
	   gie_flag = FALSE;
   }
#ifdef INV    // idle is TTL logic zero
#asm
   BCF STATUS, RP0
   MOVLW 9
   MOVWF n
   BCF STATUS, C

SER_CHAR_1:

   BTFSS STATUS, C		// 4 ~
   BSF PORTC, TxData
   BTFSC STATUS, C
   BCF PORTC, TxData
   MOVLW 31				// 1 ~
   MOVWF dly			// 1 ~

SER_CHAR_2:
   DECFSZ dly, F			// 31 * 3 ~
   GOTO SER_CHAR_2		// 1 ~
   RRF ch, F			// 1 ~
   DECFSZ n, F			// 1 ~
   GOTO SER_CHAR_1	// next bit	// 2 ~

   BCF PORTC, TxData	// idle between characters
   CLRWDT
   MOVLW 96
   MOVWF dly

SER_CHAR_3:
   DECFSZ dly, F
   GOTO SER_CHAR_3
   CLRWDT
#endasm

#else  // idle is TTL logic one
#asm
   BCF STATUS, RP0
   MOVLW 9
   MOVWF n
   BCF STATUS, C

SER_CHAR_1:

   BTFSS STATUS, C
   BCF PORTC, TxData
   BTFSC STATUS, C
   BSF PORTC, TxData
   MOVLW 32
   MOVWF dly

SER_CHAR_2:
   DECFSZ dly, F
   GOTO SER_CHAR_2
   RRF ch, F
   DECFSZ n, F
   GOTO SER_CHAR_1

   BSF PORTC, TxData // back to idle state
   CLRWDT
   MOVLW 96
   MOVWF dly

SER_CHAR_3:
   DECFSZ dly, F
   GOTO SER_CHAR_3
   CLRWDT
#endasm
#endif
    delay_ms(5);
    if(gie_flag)	// if interrupts were enabled, enable them again
    {
		gie = 1;
	}
}

byte num_to_char(byte val) // converts val to hex character
{
   byte ch;
   if (val < 10)
   {
     ch=val+'0';
   }
   else
   {
     val=val-10;
     ch=val + 'A';
   }
   return(ch);
}

void ser_new_line(void) // outputs 0x0d, 0x0a
{
   ser_char(0x0d);
   delay_ms(10); // give the PIC-n-LCD time to perform the
   ser_char(0x0a); // new line function
   delay_ms(10);
}

void ser_hex_byte(byte val)
{
   byte d;
   char ch;
   d = val >> 4;
   ch = num_to_char(d); // high nibble
   ser_char(ch);
   d = val & 0xf;
   ch = num_to_char(d); // low nibble
   ser_char(ch);
}

void lcd_cursor_pos(byte line, byte pos)
{
	const byte a[4] = {0, 64, 20, 84};
	ser_char(0x01);	// unique to PIC-n-LCD
    ser_char(a[line]+pos);
}


void ser_out_str(byte *s)
{
   while(*s)
   {
      ser_char(*s);
      ++s;
   }
}


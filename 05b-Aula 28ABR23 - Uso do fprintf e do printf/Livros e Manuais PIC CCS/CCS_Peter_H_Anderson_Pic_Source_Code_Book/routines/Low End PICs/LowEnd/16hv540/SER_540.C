// Program SER_540C. PIC16HV540 and similar (CCS PCB)
//
// This is a collection of routines to interface with a PIC-n-LCD
// or similar serial LCD capable of 9600 baud, inverted, no parity.
//
//  16HV540      PIC-n-LCD (or PC Com Port)
//
// RA0 (term 17) ---------------> Serin In (term 3)
//
// void ser_init(void); // inits PIC-n-LCD, sends 0x0c with a delay
// void ser_char(byte c); // outputs character c, 9600 baud,
// void ser_new_line(void); // outputs 0x0d, 0x0a

// void ser_out_str(byte *s);  // output null terminated str
// void ser_hex_byte(byte val); // output val in two digit hex
// void ser_dec_byte(byte val, int digits);
// output val in dec to significant figures specified
//  by digits.  For example, if val is 014, specifying
//  digits as 3, will cause "014".  Specifying digits as 2
//  will cause "14" and specifying digits as 1 will cause
//  "4"
// byte num_to_char(byte val); // converts val in range of
    // 0 - 15 to hex character
//
// copyright, Peter H. Anderson, Elmore, VT, July, '01


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

void ser_init(void) // sets TxData in idle state
{
#ifdef INV

#asm
    BCF PORTA, TxData // idle at logic zero
    BCF DIRA, TxData
    MOVF DIRA, W
 TRIS PORTA
#endasm

#else
#asm
    BSF PORTA, TxData
    BCF DIRA, TxData
    MOVF DIRA, W
 TRIS PORTA
#endasm
#endif
   ser_char(0x0c); // for PIC-n-LCD from BG Micro
   delay_ms(250);
}
void ser_char(byte ch) // serial output 9600 baud
{
   //byte n, dly;
   ser_char_ch = ch;    // copy to global
     // start bit + 8 data bits
#ifdef INV
#asm
       MOVLW 9
       MOVWF ser_char_n
       BCF STATUS, CY

SER_CHAR_1:
       BTFSS STATUS, CY
       BSF PORTA, TxData
       BTFSC STATUS, CY
       BCF PORTA, TxData
       MOVLW 32
       MOVWF ser_char_dly

SER_CHAR_2:
       DECFSZ ser_char_dly, F
       GOTO SER_CHAR_2
       RRF ser_char_ch, F
       DECFSZ ser_char_n, F
       GOTO SER_CHAR_1

       BCF PORTA, TxData
       CLRWDT
       MOVLW 96
       MOVWF ser_char_dly

SER_CHAR_3:
       DECFSZ ser_char_dly, F
       GOTO SER_CHAR_3
       CLRWDT
#endasm

#else // true

#asm
       MOVLW 9
       MOVWF n
       BCF STATUS, CY

SER_CHAR_1:

       BTFSS STATUS, CY
       BCF PORTA, TxData
       BTFSC STATUS, CY
       BSF PORTA, TxData
       MOVLW 32
       MOVWF dly

SER_CHAR_2:
       DECFSZ dly, F
       GOTO SER_CHAR_2
       RRF ch, F
       DECFSZ n, F
       GOTO SER_CHAR_1

       BSF PORTA, TxData
       CLRWDT
       MOVLW 96
       MOVWF dly

SER_CHAR_3:
       DECFSZ dly, F
       GOTO SER_CHAR_3
       CLRWDT
#endasm
#endif
}

void ser_new_line(void) // outputs 0x0d, 0x0a
{
   ser_char(0x0d);
   delay_ms(10); // give the PIC-n-LCD time to perform the
   ser_char(0x0a); // new line function
   delay_ms(10);
}


void ser_out_str(byte s)
{
   while(*s)
   {
      ser_char(*s);
      ++s;
   }
}

void ser_hex_byte(byte val) // displays val in hex format
{
   byte ch;
   ch = num_to_char((val>>4) & 0x0f);
   ser_char(ch);
   ch = num_to_char(val&0x0f);
   ser_char(ch);
}

void ser_dec_byte(byte val, byte digits)
// displays byte in decimal as either 1, 2 or 3 digits
{
   byte d;
   byte ch;
   if (digits == 3)
   {
      d=val/100;
      ch=num_to_char(d);
      ser_char(ch);
   }
   if (digits >1) // take the two lowest digits
   {
       val=val%100;
       d=val/10;
       ch=num_to_char(d);
       ser_char(ch);
   }
   if (digits == 1) // take the least significant digit
   {
       val = val%100;
   }

   d=val % 10;
   ch=num_to_char(d);
   ser_char(ch);
}

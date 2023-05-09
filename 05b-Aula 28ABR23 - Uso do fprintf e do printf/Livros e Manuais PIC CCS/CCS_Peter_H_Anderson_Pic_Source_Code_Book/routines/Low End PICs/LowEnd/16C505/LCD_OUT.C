// Program LCD_OUT.C (PIC16C505), CCS PCB
//
// This collection of routines provides a direct interface with a 20X4 Optrex
// DMC20434 LCD to permit the display of text.  This uses PIC outputs PORTC,
// bits 0 - 5
//
// Routine lcd_init() places the LCD in a 4-bit transfer mode, selects
// the 5X8 font, blinking block cursor, clears the LCD and places the
// cursor in the upper left.
//
// Routine lcd_char(byte c) displays ASCII value c on the LCD.  Note that
// this permits the use of printf statements; printf(lcd_char, "T=%f", T_F).
//
// Routine lcd_dec_byte() displays a quantity  with a specified number of
// digits. Routine lcd_hex_byte() displays a byte in two digit hex format.
// Routine lcd_str() outputs the string. In many applications, these may
// be used in place of printf statements.
//
// Routine lcd_clr() clears the LCD and locates the cursor at the upper
// left.  lcd_clr_line() clears the specified line and places the cursor
// at the beginning of that line.  Lines are numbered 0, 1, 2, 3.
//
// Routine lcd_cmd_byte() may be used to send a command to the lcd.
//
// Routine lcd_cursor_pos() places the cursor on the specified line (0-3)
// at the specified position (0 - 19).
//
// The other routines are used to implement the above.
//
//   lcd_data_nibble() - used to implement lcd_char.  Outputs the
//   specified nibble.
//
//   lcd_cmd_nibble() - used to implement lcd_cmd_byte.  The difference
//   between lcd_data_nibble and lcd_cmd_nibble is that with data, LCD
//   input RS is at a logic one.
//
//   num_to_char() - converts a digit to its ASCII equivalent.
//
//   PIC16C505               DMC20434
//
//    PORTC5 (term 5) -------------------- EN (CLK) (term 6)
//    PORTC4 (term 6) -------------------- RS (DAT/CMD) (term 4)
//                         GRD ----------- RW (term 5)
//
//    PORTC3 (term 7) -------------------- DB7 (term 14)
//    PORTC2 (term 8) -------------------- DB6 (term 13)
//    PORTC1 (term 9) -------------------- DB5 (term 12)
//    PORTC0 (term 10) ------------------- DB4 (term 11)
//
//                          +5 VDC ------- VCC (term 2)
//                                   |
//                                  4.7K
//                                   |____ VEE (term 3)
//          |
//                                  330
//                                   |
//                           GRD ---------- GRD (term 1)
//
// copyright, Peter H. Anderson, Brattleboro, VT, July, '01


void lcd_char(byte c)      // displays ASCII character c to LCD
{
    lcd_data_nibble(c>>4);    // high byte followed by low
    lcd_data_nibble(c&0x0f);
    delay_ms(1);
}

void lcd_data_nibble(byte c)  // RS is at logic one for data
{
   PORTC = c | 0x10;
   delay_ms(1);
   PORTC = c | 0x10 | 0x20;
   delay_ms(1);
   PORTC = c | 0x10;
}

void lcd_cmd_byte(byte c)  // used for sending byte commands
{
    lcd_cmd_nibble(c>>4);    // high byte followed by low
    lcd_cmd_nibble(c&0x0f);
    delay_ms(1);
}

void lcd_cmd_nibble(byte c)   // RS is at logic zero for commands
{
   PORTC = c;
   delay_ms(1);
   PORTC = c | 0x20;
   delay_ms(1);
   PORTC = c;

}

void lcd_init(void)
{
   PORTC = 0x00;
   DIRC = 0x00;
#asm
   MOVF DIRC, W
   TRIS PORTC
#endasm

   lcd_cmd_nibble(0x03);               // configure LCD in 4-bit transfer mode
   delay_ms(5);
   lcd_cmd_nibble(0x03);
   delay_ms(5);
   lcd_cmd_nibble(0x03);
   delay_ms(5);
   lcd_cmd_nibble(0x02);
   delay_ms(5);
   lcd_cmd_byte(0x0f);
   lcd_cmd_byte(0x01);
}

void lcd_clr(void)      // clear LCD and cursor to upper left
{
    lcd_cmd_byte(0x01);
}

void lcd_clr_line(byte line)   // clear indicated line and leave
                        // cursor at the beginning of the line
{
    byte n;
    lcd_cursor_pos(line, 0);
    for (n=0; n<20; n++)
    {
        lcd_char(' ');
    }
    lcd_cursor_pos(line, 0);
}

void lcd_cursor_pos(byte line, byte pos)
                  // psition cursor on line 0 .. 3, pos  0 .. 19
{
    const byte a[4] = {0x80, 0xc0, 0x94, 0xd4};
    lcd_cmd_byte(a[line]+pos);
}

void lcd_str(char *s)
{
   byte n=0;
   while(s[n])
   {
      lcd_char(s[n]);
      ++n;
   }
}

#separate void lcd_dec_byte(byte val, byte digits)
// displays byte in decimal as either 1, 2 or 3 digits
{
   byte d;
   char ch;
   if (digits == 3)
   {
      d=val/100;
      ch=num_to_char(d);
      lcd_char(ch);
   }
   if (digits >1) // take the two lowest digits
   {
       val=val%100;
       d=val/10;
       ch=num_to_char(d);
       lcd_char(ch);
   }
   if (digits == 1) // take the least significant digit
   {
       val = val%100;
   }

   d=val % 10;
   ch=num_to_char(d);
   lcd_char(ch);
}

#separate void lcd_hex_byte(byte val)
{
   byte d;
   char ch;
   d = val >> 4;
   ch = num_to_char(d); // high nibble
   lcd_char(ch);
   d = val & 0xf;
   ch = num_to_char(d); // low nibble
   lcd_char(ch);
}

char num_to_char(byte val) // converts val to hex character
{
   char ch;
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





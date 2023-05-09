// SER_14.H
//
// copyright, Peter H. Anderson, Elmore, VT, June, '02

void ser_init(void);
void ser_char(byte ch);
void ser_new_line(void);

byte num_to_char(byte val);

void ser_hex_byte(byte val);
void lcd_cursor_pos(byte line, byte pos);

void ser_out_str(byte *s);

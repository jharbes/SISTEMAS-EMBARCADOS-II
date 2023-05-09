// LCD_OUT.H (PIC16C505), CCS PCB
//
// copyright, Peter H. Anderson, Brattleboro, VT, '01

void lcd_char(byte c);
void lcd_data_nibble(byte c);
void lcd_cmd_byte(byte c);
void lcd_cmd_nibble(byte c);

void lcd_init(void);
void lcd_clr(void);
void lcd_clr_line(byte line);
void lcd_cursor_pos(byte line, byte pos);

void lcd_str(char *s);
#separate void lcd_dec_byte(byte val, byte digits);
#separate void lcd_hex_byte(byte val);
char num_to_char(byte val);

// ser_87x.h
//
// copyright, Peter H. Anderson, Georgetown, SC, Mar, '01

void ser_init(void);
void asynch_enable(void);
void asynch_disable(void);
void ser_char(char ch);
void ser_hex_byte(byte val);
void ser_dec_byte(byte val, byte digits);
void ser_out_str(char *s);
char num_to_char(byte val);

char ser_get_ch(long t_wait);
byte ser_get_str_1(char *p_chars, long t_wait_1, long t_wait_2, char term_char);
byte ser_get_str_2(char *p_chars, long t_wait_1, long t_wait_2, byte num_chars);


// These are for the BasicX Serial LCD Plus
void ser_lcd_init(void);
void ser_lcd_backlight_off(void);
void ser_lcd_backlight_on(void);
void ser_lcd_set_backlight(byte v);
void ser_lcd_set_contrast(byte v);
void ser_lcd_set_cursor_style(byte v);
void ser_lcd_clr_all(void);
void ser_lcd_clr_line(byte line);
void ser_lcd_cursor_pos(byte col, byte line);
void ser_lcd_set_beep_freq(byte v);
void ser_lcd_beep(void);

// ser_18c.h
//
// copyright, Peter H. Anderson, Baltimore, MD, Dec, '01

void ser_init(void);
void asynch_enable(void);
void asynch_disable(void);
void ser_char(char ch);
void ser_newline(void);
void ser_hex_byte(byte val);
void ser_dec_byte(byte val, byte digits);
void ser_out_str(char *s);
char num_to_char(byte val);

char ser_get_ch(long t_wait);
byte ser_get_str_1(char *p_chars, long t_wait_1, long t_wait_2, char term_char);
byte ser_get_str_2(char *p_chars, long t_wait_1, long t_wait_2, byte num_chars);



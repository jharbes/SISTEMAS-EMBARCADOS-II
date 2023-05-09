// ser_509.h
//
// copyright, Peter H Anderson, Elmore, VT, July, '01

void ser_init(void);
void ser_char(byte ch);
void ser_new_line(void);
void ser_out_str(byte *s);
void ser_hex_byte(byte val);
void ser_dec_byte(byte val, byte digits);

byte num_to_char(byte val);

byte ser_char_ch, ser_char_dly, ser_char_n;

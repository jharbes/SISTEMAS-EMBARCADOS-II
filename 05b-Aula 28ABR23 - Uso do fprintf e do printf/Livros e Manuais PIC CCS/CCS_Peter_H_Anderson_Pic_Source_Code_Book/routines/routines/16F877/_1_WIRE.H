// _1_WIRE.H
//
// Header file associated with _1_WIRE.C.
//
// copyright, Peter H. Anderson, South Hill, VA, Mar, '01

byte const mask_one[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
byte const mask_zero[8] = {0xfe, 0xfd, 0xfb, 0xf7, 0xef, 0xdf, 0xbf, 0x7f};

byte _1w_init(byte bit_pos);
byte _1w_in_byte(byte bit_pos);
void _1w_out_byte(byte bit_pos, byte d);
void _1w_strong_pull_up(byte bit_pos);


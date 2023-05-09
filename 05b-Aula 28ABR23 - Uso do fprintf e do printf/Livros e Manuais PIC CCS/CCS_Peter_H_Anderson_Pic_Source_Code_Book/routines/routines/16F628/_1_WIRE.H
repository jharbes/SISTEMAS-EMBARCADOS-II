// _1_WIRE.H (PIC16F628)

byte const mask_one[4] = {0x10, 0x20, 0x40, 0x80};
byte const mask_zero[8] = {0xef, 0xdf, 0xbf, 0x7f};

byte _1w_init(byte sensor);
byte _1w_in_byte(byte sensor);
void _1w_out_byte(byte sensor, byte d);
void _1w_strong_pull_up(byte sensor);


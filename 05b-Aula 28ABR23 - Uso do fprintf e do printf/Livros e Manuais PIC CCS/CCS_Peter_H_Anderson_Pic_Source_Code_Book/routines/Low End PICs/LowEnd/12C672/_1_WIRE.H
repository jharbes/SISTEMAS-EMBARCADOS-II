// _1_WIRE.H (PIC12C672), CCS PCM
//
// Dallas 1-wire routines
//
// copyright Peter H. Anderson, Elmore, VT, July, '01

void _1w_init(byte io);
byte _1w_in_byte(byte io);
void _1w_out_byte(byte io, byte d);
void _1w_pin_hi(byte io);
void _1w_pin_low(byte io);
void _1w_strong_pull_up(byte io);

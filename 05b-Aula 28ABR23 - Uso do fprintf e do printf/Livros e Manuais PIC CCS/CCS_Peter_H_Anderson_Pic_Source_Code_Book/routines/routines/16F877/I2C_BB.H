// I2C_BB.H
//
// Header file for I2C bit bang routines.
//
// copyright, Peter H. Anderson, Baltimore, MD, Feb, '01

void i2c_setup_bb(void);
byte i2c_in_byte_bb(byte ack);
void i2c_out_byte_bb(byte o_byte);
void i2c_start_bb(void);
void i2c_stop_bb(void);

void i2c_high_sda(void);
void i2c_low_sda(void);
void i2c_high_scl(void);
void i2c_low_scl(void);

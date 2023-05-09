// common i2c routines
void i2c_master_setup(void);
byte i2c_master_in_byte(byte ack);
void i2c_master_out_byte(byte o_byte);
void i2c_master_start(void);
void i2c_master_repeated_start(void);
void i2c_master_stop(void);

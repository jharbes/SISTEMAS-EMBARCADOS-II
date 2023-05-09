// I2C_BB.C
//
// This is a collection of low level I2C routines which are implemented
// using "bit bang" as opposed to using a MSSP module.
//
// This is not a standalone program but is included in another file.
//
// Note that SDA_PIN, SCL_PIN, SDA_DIR and SCL_DIR must be defined in the
// rotuine whigh uses this module.
//
// void i2c_setup_bb(void);	 - configures both SDA and SCL leads in high Z
// byte i2c_in_byte_bb(byte ack); - receives from slave and returns byte,
// 		most sig byte first.  If variable ack is "1", the master sends a
//	    logic zero bit after the byte is received.  Otherwise, a high Z bit
//		is sent.
//
// void i2c_out_byte_bb(byte o_byte); - outputs the byte, ms bit first, followed
//		by a high Z bit to give the slave the opportunity to acknowledge.
//
// void i2c_start_bb(void); - brings SDA to zero while SCL is high
// void i2c_stop_bb(void); - brings SDA high while SCL is high
//
// void i2c_high_sda(void); - high Z on SDA
// void i2c_low_sda(void); - logic zero on SDA
// void i2c_high_scl(void); - high Z on SCL
// void i2c_low_scl(void); - logic zero on SCL
//
// copyright, Peter H. Anderson, Baltimore, MD, Feb, '01

void i2c_setup_bb(void)
{
	i2c_high_sda();
	i2c_high_scl();
}

byte i2c_in_byte_bb(byte ack)
{
   byte i_byte, n;
   i2c_high_sda();
   for (n=0; n<8; n++)
   {
      i2c_high_scl();

      if (SDA_PIN)
      {
         i_byte = (i_byte << 1) | 0x01; // msbit first
      }
      else
      {
         i_byte = i_byte << 1;
      }
      i2c_low_scl();
   }

   if (ack)					// if ack is desired, bring SDA low for a clock pulse
   {
	   i2c_low_sda();
   }
   else
   {
	   i2c_high_sda();		// not really necessary as it is already high
   }

   i2c_high_scl();			// clock for ack or nack
   i2c_low_scl();

   i2c_high_sda();			// be sure to leave rotuine with SDA high

   return(i_byte);
}

void i2c_out_byte_bb(byte o_byte)
{
   byte n;
   for(n=0; n<8; n++)
   {
      if(o_byte&0x80)
      {
         i2c_high_sda();
      }
      else
      {
         i2c_low_sda();
      }
      i2c_high_scl();
      i2c_low_scl();
      o_byte = o_byte << 1;
   }
   i2c_high_sda();

   i2c_high_scl();		// nack
   i2c_low_scl();
}

void i2c_start_bb(void)
{
   i2c_low_scl();
   i2c_high_sda();
   i2c_high_scl();	// bring SDA low while SCL is high
   i2c_low_sda();
   i2c_low_scl();
}

void i2c_stop_bb(void)
{
   i2c_low_scl();
   i2c_low_sda();
   i2c_high_scl();
   i2c_high_sda();  // bring SDA high while SCL is high
   // idle is SDA high and SCL high
}

void i2c_high_sda(void)
{
   // bring SDA to high impedance
   SDA_DIR = 1;
   // delay_10us(5);
}

void i2c_low_sda(void)
{
   SDA_PIN = 0;
   SDA_DIR = 0;  // output a hard logic zero
   // delay_10us(5);
}

void i2c_high_scl(void)
{
   SCL_DIR = 1;   // high impedance
   // delay_10us(5);
}

void i2c_low_scl(void)
{
   SCL_PIN = 0;
   SCL_DIR = 0;
   // delay_10us(5);
}




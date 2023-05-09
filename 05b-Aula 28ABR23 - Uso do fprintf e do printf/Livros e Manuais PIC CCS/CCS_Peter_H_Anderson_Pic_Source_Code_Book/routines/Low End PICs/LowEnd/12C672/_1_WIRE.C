// _1_wire.c (PIC16C672), CCS PCM
//
// Standard 1-Wire routines.
//
// copyright, Peter H. Anderson, baltimore, MD, Aug, '00

void _1w_init(byte io)
{
   _1w_pin_hi(io);	// be sure DQ is high
   _1w_pin_low(io);
   delay_10us(50);	// low for 500 us

   _1w_pin_hi(io);
   delay_10us(50);
}

byte _1w_in_byte(byte io)
{
   byte n, i_byte, temp, mask_1, mask_0;
   mask_1 = 0x01<<io;
   mask_0 = ~mask_1;

   for (n=0; n<8; n++)
   {
      GPIO = GPIO & mask_0;
      TRIS = TRIS & mask_0;		// momentary low
      TRIS = TRIS | mask_1;
#asm
      NOP
      NOP
      NOP
      NOP
#endasm
      temp = GPIO;		// read port
      if (temp & mask_1)
      {
        i_byte=(i_byte>>1) | 0x80;	// least sig bit first
      }
      else
      {
        i_byte=i_byte >> 1;
      }
      delay_10us(6);
   }
   return(i_byte);
}

void _1w_out_byte(byte io, byte d)
{
   byte n, mask_1, mask_0;
   mask_1 = 0x01 << io;
   mask_0 = ~mask_1;

   for(n=0; n<8; n++)
   {
      if (d&0x01)
      {
         GPIO = GPIO & mask_0;
         TRIS = TRIS & mask_0;		// momentary low
         TRIS = TRIS | mask_1;		// and then high for 60 us
         delay_10us(6);
      }

      else
      {
         GPIO = GPIO & mask_0;		// low for 60 us
         TRIS = TRIS & mask_0;
         delay_10us(6);
         TRIS = TRIS | mask_1;
      }
      d=d>>1;
   }
}

void _1w_pin_hi(byte io)
{
   byte mask_1;
   mask_1 = 0x01 << io;
   TRIS = TRIS | mask_1;
}

void _1w_pin_low(byte io)
{
   byte mask_0;
   mask_0 = ~(0x01 << io);
   GPIO = GPIO & mask_0;	// 0 in bit sensor
   TRIS = TRIS & mask_0;
}

void _1w_strong_pull_up(byte io)
{
   byte mask_1, mask_0;
   mask_1 = 0x01 << io;
   mask_0 = ~mask_1;

   GPIO = GPIO | mask_1;	// output a hard logic one
   TRIS = TRIS & mask_0;

   delay_ms(750);
   TRIS = TRIS | mask_1;
}


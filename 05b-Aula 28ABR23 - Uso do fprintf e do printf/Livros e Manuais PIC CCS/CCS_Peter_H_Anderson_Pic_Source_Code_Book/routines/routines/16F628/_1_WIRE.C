// _1_WIRE.C  (PIC16F628)
//
// The following are standard 1-Wire routines for operation of 1-W devices
// on PORTB, bits 4 - 7.
//
// copyright, Peter H. Anderson, Baltimore, MD, Apr, '01

// byte const mask_one[4] = {0x10, 0x20, 0x40, 0x80};
// byte const mask_zero[8] = {0xef, 0xdf, 0xbf, 0x7f};

// byte _1w_init(byte sensor);
// byte _1w_in_byte(byte sensor);
// void _1w_out_byte(byte sensor, byte d);
// void _1w_strong_pull_up(byte sensor);

byte _1w_init(byte sensor)
{
   byte n=250, dir_in, dir_out;

   dir_in = TRISB | mask_one[sensor];
   dir_out = TRISB & mask_zero[sensor];

   TRISB = dir_in;		// be sure DQ is high
   PORTB = PORTB & mask_zero[sensor];
   TRISB = dir_out;

   delay_10us(50);	// low for 500 us

   TRISB = dir_in;

   while((PORTB & mask_one[sensor]) && (--n))	/* loop */	;

   delay_10us(50);

   if (n)
   {
      return(TRUE);
   }
   else
   {
      return(FALSE);
   }
}

byte _1w_in_byte(byte sensor)
{
   byte n, i_byte, temp, dir_in, dir_out;

   dir_in = TRISB | mask_one[sensor];
   dir_out = TRISB & mask_zero[sensor];

   PORTB = PORTB & mask_zero[sensor];

   for (n=0; n<8; n++)
   {

      TRISB = dir_out;	// bring low pin low
      TRISB = dir_in; 	// and back to high Z
#asm
      NOP
 	  NOP
	  NOP
	  NOP
#endasm
      temp = PORTB;		// read port
      if (temp & mask_one[sensor])
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

void _1w_out_byte(byte sensor, byte d)
{
   byte n, dir_in, dir_out;

   dir_in = TRISB | mask_one[sensor];
   dir_out = TRISB & mask_zero[sensor];

   PORTB = PORTB & mask_zero[sensor];

   for(n=0; n<8; n++)
   {
      if (d&0x01)
      {
         TRISB = dir_out;		// momentary low
         TRISB = dir_in;
         delay_10us(6);
      }

      else
      {
          TRISB = dir_out;
	      delay_10us(6);
          TRISB = dir_in;
      }
      d=d>>1;
   }
}

void _1w_strong_pull_up(byte sensor)
{
   byte dir_in, dir_out;

   dir_in = TRISB | mask_one[sensor];
   dir_out = TRISB & mask_zero[sensor];


   PORTB = PORTB | mask_one[sensor];	// hard logic one
   TRISB = dir_out;
   delay_ms(750);
   TRISB = dir_in;						// and bacl to high impedance

   PORTB = PORTB & mask_zero[sensor];
}


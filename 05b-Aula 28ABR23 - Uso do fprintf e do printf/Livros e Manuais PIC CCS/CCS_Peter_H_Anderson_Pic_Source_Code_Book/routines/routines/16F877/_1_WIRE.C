// The following are standard 1-Wire routines.
//
// copyright, Peter H. Anderson, South Hill, VA, Mar, '01


// byte const mask_one[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
// byte const mask_zero[8] = {0xfe, 0xfd, 0xfb, 0xf7, 0xef, 0xdf, 0xbf, 0x7f};

// byte _1w_init(byte bit_pos);
// byte _1w_in_byte(byte bit_pos);
// void _1w_out_byte(byte bit_pos, byte d);
// void _1w_strong_pull_up(byte bit_pos);


byte _1w_init(byte bit_pos)
{
   byte n=250, dir_in, dir_out;

   dir_in = TRISD | mask_one[bit_pos];
   dir_out = TRISD & mask_zero[bit_pos];

   TRISD = dir_in;		// be sure DQ is high
   PORTD = PORTD & mask_zero[bit_pos];
   TRISD = dir_out;

   delay_10us(50);	// low for 500 us

   TRISD = dir_in;

   while((PORTD & mask_one[bit_pos]) && (--n))	/* loop */	;

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


byte _1w_in_byte(byte bit_pos)
{
   byte n, i_byte, temp, dir_in, dir_out;

   dir_in = TRISD | mask_one[bit_pos];
   dir_out = TRISD & mask_zero[bit_pos];

   PORTD = PORTD & mask_zero[bit_pos];

   for (n=0; n<8; n++)
   {

      TRISD = dir_out;	// bring low pin low
      TRISD = dir_in; 	// and back to high Z
#asm
      NOP
 	  NOP
	  NOP
	  NOP
#endasm
      temp = PORTD;		// read port
      if (temp & mask_one[bit_pos])
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

void _1w_out_byte(byte bit_pos, byte d)
{
   byte n, dir_in, dir_out;

   dir_in = TRISD | mask_one[bit_pos];
   dir_out = TRISD & mask_zero[bit_pos];

   PORTD = PORTD & mask_zero[bit_pos];

   for(n=0; n<8; n++)
   {
      if (d&0x01)
      {
         TRISD = dir_out;		// momentary low
         TRISD = dir_in;
         delay_10us(6);
      }

      else
      {
          TRISD = dir_out;
	      delay_10us(6);
          TRISD = dir_in;
      }
      d=d>>1;
   }
}

void _1w_strong_pull_up(byte bit_pos)
{
   byte dir_in, dir_out;

   dir_in = TRISD | mask_one[bit_pos];
   dir_out = TRISD & mask_zero[bit_pos];


   PORTD = PORTD | mask_one[bit_pos];	// hard logic one
   TRISD = dir_out;
   delay_ms(750);
   TRISD = dir_in;						// and bacl to high impedance

   PORTD = PORTD & mask_zero[bit_pos];
}


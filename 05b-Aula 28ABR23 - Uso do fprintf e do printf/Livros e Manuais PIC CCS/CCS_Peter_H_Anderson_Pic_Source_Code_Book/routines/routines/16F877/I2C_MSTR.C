// I2C_MSTR.C
//
//
// copyright, Peter H. Anderson, Baltimore, MD, Jan, '01

void i2c_master_setup(void)
{
   trisc3 = 1;	// scl - term 18
   trisc4 = 1;	// sda - term 23
   stat_smp = 1;
   stat_cke = 0;	// input levels conform to i2c

   SSPADD = 3;
        //  1.0 MHz / (SSPADD + 1)	- 250 kHz

   sspm3 = 1;	// i2c master mode
   sspm2 = 0;
   sspm1 = 0;
   sspm0 = 0;

   sspen = 1;	// enable ssp
}

void i2c_master_start(void)
{
   byte n=50;
   sen = 1;
   while(sen && --n)		;	// n used to avoid infinite loop
#ifdef TEST
   n=n;	// for debugging
   if(n==0)
   {
      blip_led(1);
      delay_ms(500);
   }
#endif
}

void i2c_master_repeated_start(void)
{
   byte n=50;
   rsen = 1;
   while(rsen && --n)		;
#ifdef TEST
   n=n;	// for debugging
   if(n==0)
   {
      blip_led(1);
      delay_ms(500);
   }
#endif
}

void i2c_master_stop(void)
{
   byte n=50;
   pen = 1;
   while(pen && --n)		;
#ifdef TEST
   n=n;	// for debugging
   if(n==0)
   {
      blip_led(2);
      delay_ms(500);
   }
#endif
}


void i2c_master_out_byte(byte o_byte)
{
   byte n=50;
   SSPBUF = o_byte;
   while(stat_bf && --n)	;
#ifdef TEST
   n=n;	// for debugging
#endif
   n=50;
   while(ackstat && --n)	;
#ifdef TEST
   n=n;	// for debugging
   if(n==0)
   {
      blip_led(3);
      delay_ms(500);
   }
#endif
}

byte i2c_master_in_byte(byte ack)
{
   byte n=50;
   rcen = 1;
   delay_ms(1);
   while(rcen & --n)  ; 			//(!STAT_BF && --n)	;
#ifdef TEST
   n=n;	// for debugging
   if(n==0)
   {
      blip_led(4);
      delay_ms(500);
   }
#endif
   n= 50;
   ackdt = ack ? 0 : 1;		// nack or ack
   acken = 1;
   while(acken & --n)	;
   return(SSPBUF);
}


#ifdef TEST
void blip_led(byte n)
{
   static byte first = TRUE;
   byte m;
   if (first)
   {
      porte0 = 0;
      trise0 = 0;
      first = FALSE;

   }
   for(m=0; m<n; m++)
   {
      porte0 = 1;
      delay_ms(100);
      porte0 = 0;
      delay_ms(100);
   }
}
#endif



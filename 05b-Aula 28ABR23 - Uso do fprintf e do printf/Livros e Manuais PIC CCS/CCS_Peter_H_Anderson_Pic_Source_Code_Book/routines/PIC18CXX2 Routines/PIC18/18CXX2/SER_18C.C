// Program SER_18C.C
//
// This is a collection of routines to interface with a PC COM Port
// via a DS275 or MAX232 transceiver
//
// It also includes routines to receive characters and character strings
// on RC.7 (term 26).  9600 baud, noninverted, no parity.
//
//  PIC18C452   			          PC COM Port
//
// RC6/TX (term 25) -------     -------> Rx (term 2)
//
//                     MAX232 or DS275
//
// RC7/RX (term 26) <------     ------- TX (term 3)
//
// copyright, Peter H. Anderson, Baltimore, MD, Dec, '01
//
// void ser_init(void);
// void asynch_enable(void);
// void asynch_disable(void);
// void ser_char(char ch);
// void ser_newline(void);
// void ser_hex_byte(byte val);
// void ser_dec_byte(byte val, byte digits);
// void ser_out_str(char *s);
// char num_to_char(byte val);

// char ser_get_ch(long t_wait);
// byte ser_get_str_1(char *p_chars, long t_wait_1, long t_wait_2, char term_char);
// byte ser_get_str_2(char *p_chars, long t_wait_1, long t_wait_2, byte num_chars);



void ser_init(void) // sets TxData in idle state
{
   asynch_enable();
}

void asynch_enable(void)
{
   trisc6 = 1; // make tx (term 25 an input)
   trisc7 = 1; // rx (term 26) input

   sync = 0;   // asynchronous
   brgh = 1 ;  // baud rate generator high speed
   SPBRG = 25;  // 9600 with 4.0 MHz clock
		// SPGRG = 129 for 9600 baud with 20.0 MHz clock

   spen = 1;    // serial port enabled

   txen = 1;  // as appropriate
   cren = 1;
}

void asynch_disable(void)
{
   txen = 0;
   cren = 0;
   spen = 0;
}

void ser_char(char ch) // no interrupts
{
   byte n = 250;

   delay_ms(5);		//  required for Serial LCD+
   while(!txif && --n)	/* txif goes to one when buffer is empty */
   {
	   delay_10us(1);
   }
   TXREG = ch;    	//  This clears txif
}

void ser_newline(void)
{
   ser_char(0x0d);
   ser_char(0x0a);
}   

void ser_hex_byte(byte val) // displays val in hex format
{
   char ch;
   ch = num_to_char((val>>4) & 0x0f);
   ser_char(ch);
   ch = num_to_char(val&0x0f);
   ser_char(ch);
}

void ser_dec_byte(byte val, byte digits)
// displays byte in decimal as either 1, 2 or 3 digits
{
   byte d;
   char ch;
   if (digits == 3)
   {
      d=val/100;
      ch=num_to_char(d);
      ser_char(ch);
   }
   if (digits >1) // take the two lowest digits
   {
       val=val%100;
       d=val/10;
       ch=num_to_char(d);
       ser_char(ch);
   }
   if (digits == 1) // take the least significant digit
   {
       val = val%100;
   }

   d=val % 10;
   ch=num_to_char(d);
   ser_char(ch);
}

void ser_out_str(char *s)
{
	byte n = 0;

    while ((s[n] != 0) && (n<20))
    {
       ser_char(s[n]);
       ++n;
    }
}

char num_to_char(byte val) // converts val to hex character
{
   char ch;
   if (val < 10)
   {
     ch=val+'0';
   }
   else
   {
     val=val-10;
     ch=val + 'A';
   }
   return(ch);
}

char ser_get_ch(long t_wait)
// returns 0xff if no char received within t_wait ms
{
   byte loop_one_ms, n;
   char ch;

   rcif=0;

   do
   {
      loop_one_ms = 100;
      do
      {			// assumed to be 10 instruction cycles
#asm			// check and adjust with NOPs as neccessary
	CLRWDT
#endasm
         if(rcif)
         {
            return(RCREG);
         }
      } while(loop_one_ms--);
   } while(t_wait--);
   return(0xff);	// if timeout
}

byte ser_get_str_1(char *p_chars, long t_wait_1, long t_wait_2, char term_char)
// assumes p_chars is dimensioned as 20 bytes
{
   byte n;
   char ch;
   ch = RCREG;		// fetch any garbage that may be in UART
   ch = RCREG;		// and throw it away
   cren=0;     // clear any errors
   cren=1;
   if((ch = ser_get_ch(t_wait_1)) == 0xff)
   {
     p_chars[0]=0;	// return null
     return(0);
   }
   else if (ch == term_char)
   {
     p_chars[0]=0;	// null terminate the string
     return(0);
   }
   else
   {
      p_chars[0]=ch;
   }

   for(n=1; n<19; n++)
   {
      if((ch = ser_get_ch(t_wait_2)) == 0xff)
      // timeout prior to receipt of terminal character
      {
         p_chars[n] = 0;	// null terminate
         return(n);
      }
      else if (ch == term_char)
      {
        p_chars[n] = 0;	// null terminate the string
        return(n);
      }
      else
      {
         p_chars[n] = ch;
      }
   }
   p_chars[n] = 0;	// null terminate
}

byte ser_get_str_2(char *p_chars, long t_wait_1, long t_wait_2, byte num_chars)
{
   byte n;
   char ch;

   ch = RCREG;		// fetch any garbage that may be in UART
   ch = RCREG;
   cren=0;        // clear any errors
   cren=1;
			// and throw it away
   if((ch = ser_get_ch(t_wait_1)) == 0xff)
   {
      p_chars[0]=0;	// return null
      return(0);
   }

   else
   {
      p_chars[0]=ch;
   }

   for(n=1; n<num_chars; n++)
   {

      if((ch = ser_get_ch(t_wait_2)) == 0xff)
      {
        p_chars[n]=0;	// return null
        return(n);
      }

      else
      {
         p_chars[n]=ch;
      }
   }
   p_chars[n]=0;	// null terminate - this is the normal termination
   return(n);
}



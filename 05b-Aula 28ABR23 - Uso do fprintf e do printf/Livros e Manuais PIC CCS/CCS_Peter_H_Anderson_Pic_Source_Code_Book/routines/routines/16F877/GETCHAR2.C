// GETCHAR2.C
//
// Illustrates the use of a circular buffer to receive characters
// using interrupts.
//
/// BX24					PIC16F877			Serial LCD+
// (term 12) ---------> RC7/RX (term 26)
//                  	RC6/TX (term 25) ------> (term 2)
//
// copyright, Peter H. Anderson, Georgetown, SC, Mar, '01

#case

#device PIC16F877 *=16 ICD=TRUE

#include <defs_877.h>
#include <ser_87x.h>
#include <delay.h>
#include <string.h>

#define TRUE !0
#define FALSE 0

#define RX_BUFF_MAX 20

void open_rx_com(void);
int get_rx_buff(byte *ch_ptr);

void set_next_line(byte *p_line);


byte rx_buff[RX_BUFF_MAX], rx_put_index, rx_get_index, rx_buff_full;
byte x;

void main(void)
{
   char ch, s[20];
   byte success, n=0, line = 0;

   ser_init();
   ser_lcd_init();

   open_rx_com();

   pspmode = 0;
   trisd7 = 0;

   while(1)
   {
	  portd7 = 1;		// blink the LED
	  delay_ms(200);
	  portd7 = 0;
	  delay_ms(200);
	  // ser_char('.');

	  while((success = get_rx_buff(&ch))==1)
	  {
      	 if (ch == 13)
		 {
		    s[n] = 0;
		    ser_lcd_clr_line(line);
		    ser_out_str(s);
		    set_next_line(&line);
		    n = 0;
		 }

		 else
		 {
	         s[n] = ch;
	         ++n;
	         if (n>=RX_BUFF_MAX)
	         {
				ser_lcd_clr_line(line);
				printf(ser_char, "Error");
				set_next_line(&line);
				n = 0;
			 }
   	     }
	 }  // of inner while

	 if(success == 0xff)
	 {
		 ser_lcd_clr_line(line);
		 printf(ser_char, "Overflow");
		 set_next_line(&line);
	 }

   } // of outter while
}

byte get_rx_buff(byte *ch_ptr)
{
   byte error_code;
   if (rx_buff_full == TRUE)
   {
	   rx_buff_full = FALSE;
	   error_code = 0xff;	// overflow of rx_buff
  	   rx_put_index = 0;
  	   rx_get_index = 0;
	   *ch_ptr = 0;		// buff was full.  returned character has no meaning
	}
	else if (rx_get_index == rx_put_index)	// there is no character
	{
	    error_code = 0;		// no character
		*ch_ptr = 0;
	}
	else
	{
	    *ch_ptr = rx_buff[rx_get_index];
		++rx_get_index;
		if (rx_get_index >= RX_BUFF_MAX)
		{
		   rx_get_index = 0;
        }
		error_code = 1;	// success
	}
	return(error_code);
}

void open_rx_com(void)
{
    char ch;
    asynch_enable();
	rx_put_index = 0;
	rx_get_index = 0;
	rx_buff_full = FALSE;
	ch = RCREG;		// get any junk that may be in the buffer
	ch = RCREG;

	rcif = 0;
    rcie = 1;
    peie = 1;
	gie = 1;
}

void set_next_line(byte *p_line)
{
	++(*p_line);
	if (*p_line == 4)
	{
		*p_line = 0;
	}
}

#int_rda rda_interrupt_handler(void)
{
   x = x;
   if (rx_buff_full == FALSE)
   {
      rx_buff[rx_put_index] = RCREG;	// fetch the character
  	  ++rx_put_index;
      if (rx_put_index >= RX_BUFF_MAX)
	  {
	     rx_put_index = 0;   // wrap around
      }
	  if (rx_put_index == rx_get_index)
	  {
	     rx_buff_full = TRUE;
	  }
   }
}

#int_default default_interrupt_handler(void)
{
}

#include <ser_87x.c>
#include <delay.c>



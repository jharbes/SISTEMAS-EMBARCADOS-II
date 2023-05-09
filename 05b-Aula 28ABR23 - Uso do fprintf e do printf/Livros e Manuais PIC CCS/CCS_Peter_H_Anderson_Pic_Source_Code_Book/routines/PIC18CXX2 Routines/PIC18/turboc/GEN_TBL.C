/* GEN_TBLE.C - Borland TurboC
**
** Opens binary file "click.wav" and reads to array and writes to text file
** "wav_dat.c" in the form.
**
** #rom 0x1000 = {0x3c}
** #rom 0x1001 = {0x45}
** etc
**
** Note that the first 100 bytes of the .wav file are discarded as these are setup bytes.
** I am uncertain as to the exact number.
**
** The resulting file may then either be pasted into of included in the CCS C file.
**
** copyright, Peter H Anderson, Baltimore, MD
*/

#include <stdio.h>

void main(void)
{

   FILE *f_binary, *f_txt;
   unsigned int n, address, num_bytes;
   unsigned char a[32000];

   if ((f_binary = fopen("click.wav", "rb")) == NULL)
   {
 	   printf("Error opening .wav file\n");
	   exit(0);
   }

   if ((f_txt = fopen("wav_dat.c", "wt")) == NULL)
   {
	   printf("Error opening output file\n");
	   exit(0);
   }

   num_bytes = fread(a, 1, 32000, f_binary);	/* read binary file into array */

   for (n=100, address = 0x1000; n<num_bytes; n++, address++)
   {
		/* output each value in the array, beginning with element 100 */
	   fprintf(f_txt, "#rom 0x%.4x = {0x%.2x}\n", address, a[n]);
   }

   fcloseall();
}

#ifdef OUTPUT

Sample of the output.

*******************

#rom 0x1000 = {0x65}
#rom 0x1001 = {0xbd}
#rom 0x1002 = {0x00}
#rom 0x1003 = {0x00}
#rom 0x1004 = {0xff}
#rom 0x1005 = {0x7f}
#rom 0x1006 = {0x8f}
#rom 0x1007 = {0xbf}
#rom 0x1008 = {0x41}
#rom 0x1009 = {0x8c}
#rom 0x100a = {0x00}
#rom 0x100b = {0x34}
#rom 0x100c = {0x72}
#rom 0x100d = {0x5d}
#rom 0x100e = {0x00}

#endif
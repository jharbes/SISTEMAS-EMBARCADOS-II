// 32KHZ.C  (PIC12C509)
//
// Continually generates nominally 32 kHz on output GP1 (term 6).
//
// Used as a nominally 32 kHz clock source for testing PIC18CXX2 routines.
//
// copyright, Peter H. Anderson, Baltimore, MD, Jan, '02

#case

#device PIC12C509 *=8

#include <defs_509.h>

void main(void)
{
   DIRS = 0x3f;
   dirs1 = 0;      // make gp1 an output
#asm
   MOVF DIRS, W
   TRIS GPIO

TOP:
      CLRWDT	// 15 ~ including the GOTO
      NOP
      NOP
      NOP
      NOP
      NOP
      NOP
      NOP
      NOP
      NOP
      NOP
      NOP

      BSF GPIO, 1

      CLRWDT	// 15 ~
      NOP
      NOP
      NOP
      NOP
      NOP
      NOP
      NOP
      NOP
      NOP
      NOP
      NOP
      NOP
      NOP
      BCF GPIO, 1
      GOTO TOP

#endasm
}


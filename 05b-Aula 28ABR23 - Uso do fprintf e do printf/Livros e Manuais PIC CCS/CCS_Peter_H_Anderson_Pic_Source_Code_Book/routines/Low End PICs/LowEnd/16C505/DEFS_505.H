// DEFS_505.H
//
// Standard definitions for PIC16C505
//
// Particularly note the declaration of static byte DIRB, DIRC and OPTIONS
//
// copyright, Peter H. Anderson, Elmore, VT, July, '01

#define byte unsigned int

#define W  0
#define F  1

//----- Register Files -----------------------------------------------------

#byte INDF =0x00
#byte TMR0      =0x01
#byte PCL       =0x02
#byte STATUS    =0x03
#byte FSR       =0x04
#byte OSCCAL    =0x05
#byte PORTB 	=0x06
#byte PORTC  	=0x07

static byte DIRB, DIRC, OPTIONS; // note global definition

// Direction Bits
// Note that DIRS is a file containing the directions.  Actually
// setting the directions requires a call to set_dirs which moves
// DIRS to W and then TRIS GPIO

#bit dirb5    =DIRB.5
#bit dirb4    =DIRB.4
#bit dirb3    =DIRB.3
#bit dirb2    =DIRB.2
#bit dirb1    =DIRB.1
#bit dirb0    =DIRB.0

#bit dirc5    =DIRC.5
#bit dirc4    =DIRC.4
#bit dirc3    =DIRC.3
#bit dirc2    =DIRC.2
#bit dirc1    =DIRC.1
#bit dirc0    =DIRC.0

#bit rb5   =PORTB.5
#bit rb4   =PORTB.4
#bit rb3   =PORTB.3
#bit rb2   =PORTB.2
#bit rb1   =PORTB.1
#bit rb0   =PORTB.0

#bit rc5   =PORTC.5
#bit rc4   =PORTC.4
#bit rc3   =PORTC.3
#bit rc2   =PORTC.2
#bit rc1   =PORTC.1
#bit rc0   =PORTC.0

#bit portb5   =PORTB.5
#bit portb4   =PORTB.4
#bit portb3   =PORTB.3
#bit portb2   =PORTB.2
#bit portb1   =PORTB.1
#bit portb0   =PORTB.0

#bit portc5   =PORTC.5
#bit portc4   =PORTC.4
#bit portc3   =PORTC.3
#bit portc2   =PORTC.2
#bit portc1   =PORTC.1
#bit portc0   =PORTC.0

//----- STATUS Bits --------------------------------------------------------

#bit rbwuf  =0x03.7
#bit pa0    =0x03.5
#bit not_to =0x03.4
#bit not_pd =0x03.3
//#bit z  =0x03.2  // probably not required in C
//#bit dc       =0x03.1  // but if they are, come up with
//#bit c =0x03.0  // something more unique than "z" and "c"


//----- OPTION Bits --------------------------------------------------------
// OPTION Bits
// Note that PTIONS is a file containing the options.  Actually
// setting the options requires a call to set_options which moves
// DIRS to W and then OPTION

#bit not_rbwu 	=OPTIONS.7
#bit not_rbpu 	=OPTIONS.6
#bit t0cs  		=OPTIONS.5
#bit t0se  		=OPTIONS.4
#bit psa  		=OPTIONS.3
#bit ps2  		=OPTIONS.2
#bit ps1  		=OPTIONS.1
#bit ps0  		=OPTIONS.0


#define ZZ 2
#define CY 0


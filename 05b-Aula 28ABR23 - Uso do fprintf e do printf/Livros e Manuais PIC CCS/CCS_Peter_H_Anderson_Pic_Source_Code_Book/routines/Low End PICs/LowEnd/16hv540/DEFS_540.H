// DEFS_540.H
//
// Standard definitions for PIC16HV540
//
// Particularly note the declaration of static byte DIRA, DIRB and OPTIONS1
// and OPTIONS2
//
// copyright, Peter H. Anderson, Elmore, VT, July, '01

#define byte unsigned int

#define W  0
#define F  1

//----- Register Files -----------------------------------------------------

#byte INDF   	=0x00
#byte TMR0      =0x01
#byte PCL       =0x02
#byte STATUS    =0x03
#byte FSR       =0x04

#byte PORTA  	=0x05
#byte PORTB  	=0x06

static byte DIRA, DIRB, OPTIONS1, OPTIONS2; // note global definition

// Direction Bits
// Note that DIRA and DIRB are files containing the directions of PORTA and PORTB.  Actually
// setting the directions requires a call to set_dirs which moves
// DIRS to W and then TRIS PORTA or TRIS PORTB

#bit dira3 =DIRA.3
#bit dira2 =DIRA.2
#bit dira1 =DIRA.1
#bit dira0 =DIRA.0

#bit dirb7  =DIRB.7
#bit dirb6  =DIRB.6
#bit dirb5  =DIRB.5
#bit dirb4  =DIRB.4
#bit dirb3  =DIRB.3
#bit dirb2  =DIRB.2
#bit dirb1  =DIRB.1
#bit dirb0  =DIRB.0

#bit ra3   =PORTA.3
#bit ra2   =PORTA.2
#bit ra1   =PORTA.1
#bit ra0   =PORTA.0

#bit rb7   =PORTB.7
#bit rb6   =PORTB.6
#bit rb5   =PORTB.5
#bit rb4   =PORTB.4
#bit rb3   =PORTB.3
#bit rb2   =PORTB.2
#bit rb1   =PORTB.1
#bit rb0   =PORTB.0

#bit porta3   =PORTA.3 // alternate definitions
#bit porta2   =PORTA.2
#bit porta1   =PORTA.1
#bit porta0   =PORTA.0

#bit portb7   =PORTB.7
#bit portb6   =PORTB.6
#bit portb5   =PORTB.5
#bit portb4   =PORTB.4
#bit portb3   =PORTB.3
#bit portb2   =PORTB.2
#bit portb1   =PORTB.1
#bit portb0   =PORTB.0

//----- STATUS Bits --------------------------------------------------------

#bit not_pcwuf  =0x03.7
#bit pa0    =0x03.5
#bit not_to =0x03.4
#bit not_pd =0x03.3
//#bit z  =0x03.2  // probably not required in C
//#bit dc       =0x03.1  // but if they are, come up with
//#bit c =0x03.0  // something more unique than "z" and "c"


//----- OPTION Bits --------------------------------------------------------
// OPTIONS1 Bits
// Note that OPTIONS1 is a file containing the options.
// Setting the options requires moving OPTIONS1 to W and then
// executing OPTION

#bit t0cs   =OPTIONS1.5
#bit t0se   =OPTIONS1.4
#bit psa    =OPTIONS1.3
#bit ps2    =OPTIONS1.2
#bit ps1    =OPTIONS1.1
#bit ps0    =OPTIONS1.0

//------ OPTION2 Bits
// OPTIONS2 Bits
// File OPTIONS2 is moved to W and then the command TRIS 0x07 is
// executed.

#bit not_pcwu 	=OPTIONS2.5
#bit not_swdten =OPTIONS2.4
#bit rl   		=OPTIONS2.3
#bit sl   		=OPTIONS2.2
#bit bodl 		=OPTIONS2.1
#bit not_boden 	=OPTIONS2.0

#define ZZ 2
#define CY 0


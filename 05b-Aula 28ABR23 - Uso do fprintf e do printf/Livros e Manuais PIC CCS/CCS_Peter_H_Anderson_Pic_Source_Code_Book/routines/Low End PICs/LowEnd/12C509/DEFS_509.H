// DEFS_509.H
//
// Standard definitions for 12C508, 12C509, 12CE518, 12CE519
//
// Particularly note the declaration of static int DIRS and OPTIONS
//
// Peter H. Anderson, July, '01

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
#byte GPIO      =0x06

static int DIRS, OPTIONS; // note global definitions

// I/O Bits

#bit sda_in =0x06.6	// for 12CE518 and 519 only
#bit gp5  =0x06.5
#bit gp4  =0x06.4
#bit gp3  =0x06.3
#bit gp2  =0x06.2
#bit gp1  =0x06.1
#bit gp0  =0x06.0

// Direction Bits
// Note that DIRS is a file containing the directions.  Actually
// setting the directions requires a call to set_dirs which moves
// DIRS to W and then TRIS GPIO

#bit dirs5    =DIRS.5
#bit dirs4    =DIRS.4
#bit dirs3    =DIRS.3
#bit dirs2    =DIRS.2
#bit dirs1    =DIRS.1
#bit dirs0    =DIRS.0


//----- STATUS Bits --------------------------------------------------------

#bit gpwuf  =0x03.7
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

#bit not_gpwu =OPTIONS.7
#bit not_gppu =OPTIONS.6
#bit t0cs =OPTIONS.5
#bit t0se =OPTIONS.4
#bit psa =OPTIONS.3
#bit ps2 =OPTIONS.2
#bit ps1 =OPTIONS.1
#bit ps0 =OPTIONS.0

// for assembly language
#define SCL 7
#define SDA 6
#define GP5 5
#define GP4 4
#define GP3 3
#define GP2 2
#define GP1 1
#define GP0 0

#define Z 2
#define CY 0


// DEFS_672.H
//
// Definition of registers and bits for PIC12C67X
//
// copyright, Peter H. Anderson, Elmore, VT, July, '01

#define byte unsigned int

#define W  0
#define F 1

//----- Register Files---------

#byte INDF  =0x00
#byte TMR0      =0x01
#byte PCL       =0x02
#byte STATUS    =0x03
#byte FSR       =0x04
#byte GPIO      =0x05
#byte PCLATH    =0x0a
#byte INTCON    =0x0b
#byte PIR1  	=0x0c
#byte ADRES  	=0x1e
#byte ADCON0 	=0x1f

#byte OPTION_REG =0x81
#byte TRISIO    =0x85
#byte TRIS     	=0x85
#byte PIE1  	=0x8c
#byte PCON      =0x8e
#byte OSCCAL 	=0x8f
#byte ADCON1    =0x9f

// ----- I/O Bits
// lower case for C, uppercase for assembly

#bit sda_in =0x05.6	// for 12CE673 and CE674
#bit gp5 =0x05.5
#bit gp4 =0x05.4
#bit gp3 =0x05.3
#bit gp2 =0x05.2
#bit gp1 =0x05.1
#bit gp0 =0x05.0

// ---- TRIS bits
// lower case for C

#bit tris5 =0x85.5
#bit tris4 =0x85.4
#bit tris3 =0x85.3
#bit tris2 =0x85.2
#bit tris1 =0x85.1
#bit tris0 =0x85.0

//----- STATUS Bits ---------------------------------------
// Used in assembly language
#define IRP     7
#define RP1     6
#define RP0     5
#define NOT_TO  4
#define NOT_PD  3
#define Z       2
#define DC      1
#define CY      0

//----- ADCON0 Bits ---------------------------------------
// lower case used by C, upper case by assembly

#bit adcs1 =0x1f.7
#bit adcs0 =0x1f.6
#bit chs1 =0x1f.4
#bit chs0 =0x1f.3

#bit adgo =0x1f.2
#bit go_done =0x1f.2
#bit adon =0x1f.0

#define ADCS1 7
#define ADCS0   6

#define CHS1    4
#define CHS0    3

#define GO_DONE 2
#define ADON    0

// ----- INTCON Bits ---------------------------------------
// lower case used for C, uppercase for assembly

#bit gie =0x0b.7
#bit peie =0x0b.6
#bit t0ie =0x0b.5
#bit inte =0x0b.4
#bit gpie =0x0b.3
#bit t0if =0x0b.2
#bit intf =0x0b.1
#bit gpif =0x0b.0

#define GIE     7
#define PEIE    6
#define T0IE    5
#define INTE    4
#define GPIE    3
#define T0IF    2
#define INTF    1
#define GPIF    0

// ----- PIR1 Bits -----------------------------------------
// lower case used for C, uppercase for assembly
#bit adif =0x0c.6

#define ADIF    6

// ----- OPTION Bits ------------------------------------------

#bit not_gppu =0x81.7
#bit intedg =0x81.6
#bit t0cs =0x81.5
#bit t0se =0x81.4
#bit psa =0x81.3
#bit ps2 =0x81.2
#bit ps1 =0x81.1
#bit ps0 =0x81.0

#define NOT_GPPU 7
#define INTEDG   6
#define T0CS     5
#define T0SE     4
#define PSA      3
#define PS2      2
#define PS1      1
#define PS0      0

// ----- PIE1 Bits ------------------------------------------
#bit adie =0x8c.6
#define ADIE    6

// ----- PCON Bits ------------------------------------------

#bit not_por  =0x8e.1
#define NOT_POR 1

// ----- OSCCAL Bits ----------------------------------------

#define CAL3    7
#define CAL2    6
#define CAL1    5
#define CAL0    4
#define CALFST  3
#define CALSLW  2

// ----- ADCON1 Bits -----------------------------------------

#bit pcfg2 =0x9f.2
#bit pcfg1 =0x9f.1
#bit pcfg0 =0x9f.0

#define PCFG2 2
#define PCFG1   1
#define PCFG0   0


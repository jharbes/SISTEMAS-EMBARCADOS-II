// DEFS_628.H (CCS PCM)
//
// Definitions of file registers and bits.
//
// PIC16F628.  These agree with Microchip.
//
// Note that bits are lower case for C, upper case for assembly.
//
// copyright Peter H. Anderson, Baltimore, MD, Mar, '01

// register definitions

#define byte int

#define W 0
#define F 1

// register files
#byte INDF          =0x00
#byte TMR0          =0x01
#byte PCL           =0x02
#byte STATUS        =0x03
#byte FSR           =0x04
#byte PORTA         =0x05
#byte PORTB         =0x06
#byte EEDATA        =0x08
#byte EEADR         =0x09
#byte PCLATH        =0x0a
#byte INTCON        =0x0b

#byte PIR1    		=0x0c

#byte TMR1L   		=0x0e
#byte TMR1H   		=0x0f
#byte T1CON   		=0x10
#byte TMR2    		=0x11
#byte T2CON   		=0x12

#byte CCPR1L  		=0x15
#byte CCPR1H  		=0x16
#byte CCP1CON 		=0x17
#byte RCSTA   		=0x18
#byte TXREG   		=0x19
#byte RCREG   		=0x1a

#byte CMCON   		=0x1f

#byte OPTION_REG    =0x81
#byte TRISA         =0x85
#byte TRISB         =0x86

#byte PIE1    		=0x8c

#byte PCON    		=0x8e

#byte PR2     		=0x92

#byte TXSTA   		=0x98
#byte SPBRG   		=0x99
#byte EEDATA  		=0x9a
#byte EEADR   		=0x9b
#byte EECON1  		=0x9c
#byte EECON2  		=0x9d

#byte VRCON   		=0x9f

#bit not_to	=0x03.4
#bit not_pd =0x03.3

#bit ra7	=0x05.7
#bit ra6	=0x05.6
#bit ra5	=0x05.5
#bit ra4	=0x05.4
#bit ra3	=0x05.3
#bit ra2	=0x05.2
#bit ra1	=0x05.1
#bit ra0	=0x05.0

#bit rb7	=0x06.7
#bit rb6	=0x06.6
#bit rb5	=0x06.5
#bit rb4	=0x06.4
#bit rb3	=0x06.3
#bit rb2	=0x06.2
#bit rb1	=0x06.1
#bit rb0	=0x06.0

// INTCON Bits for C
#bit gie 	= 0x0b.7
#bit peie	= 0x0b.6
#bit t0ie 	= 0x0b.5
#bit inte	= 0x0b.4
#bit rbie	= 0x0b.3
#bit t0if   = 0x0b.2
#bit intf   = 0x0b.1
#bit rbif   = 0x0b.0

#bit  eeif    =0x0c.7
#bit  cmif    =0x0c.6
#bit  rcif    =0x0c.5
#bit  txif    =0x0c.4
#bit  ccp1if  =0x0c.2
#bit  tmr2if  =0x0c.1
#bit  tmr1if  =0x0c.0

#bit  t1ckps1 =0x10.5
#bit  t1ckps0 =0x10.4
#bit  t1oscen =0x10.3
#bit  t1sync  =0x10.2
#bit  tmr1cs  =0x10.1
#bit  tmr1on  =0x10.0

#bit  toutps3 =0x12.6
#bit  toutps2 =0x12.5
#bit  toutps1 =0x12.4
#bit  toutps0 =0x12.3
#bit  tmr2on  =0x12.2
#bit  t2ckps1 =0x12.1
#bit  t2ckps0 =0x12.0

#bit  ccp1x   =0x17.5
#bit  ccp1y   =0x17.4
#bit  ccp1m3  =0x17.3
#bit  ccp1m2  =0x17.2
#bit  ccp1m1  =0x17.1
#bit  ccp1m0  =0x17.0

#bit  spen    =0x18.7
#bit  rx9     =0x18.6
#bit  sren    =0x18.5
#bit  cren    =0x18.4
#bit  aden   =0x18.3
#bit  ferr    =0x18.2
#bit  oerr    =0x18.1
#bit  rx9d    =0x18.0


#bit  c2out   =0x1f.7
#bit  c1out   =0x1f.6
#bit  c2inv   =0x1f.5
#bit  c1inv   =0x1f.4
#bit  cis     =0x1f.3
#bit  cm2     =0x1f.2
#bit  cm1     =0x1f.1
#bit  cm0     =0x1f.0


#bit trisa4	=0x85.4
#bit trisa3	=0x85.3
#bit trisa2	=0x85.2
#bit trisa1	=0x85.1
#bit trisa0	=0x85.0

#bit trisb7	=0x86.7
#bit trisb6	=0x86.6
#bit trisb5	=0x86.5
#bit trisb4	=0x86.4
#bit trisb3	=0x86.3
#bit trisb2	=0x86.2
#bit trisb1	=0x86.1
#bit trisb0	=0x86.0

// OPTION Bits
#bit not_rbpu	= 0x81.7
#bit intedg     = 0x81.6
#bit t0cs       = 0x81.5
#bit t0se       = 0x81.4
#bit psa        = 0x81.3
#bit ps2        = 0x81.2
#bit ps1        = 0x81.1
#bit ps0        = 0x81.0

#bit  eeie    =0x8c.7
#bit  cmie    =0x8c.6
#bit  rcie    =0x8c.5
#bit  txie    =0x8c.4
#bit  ccpie   =0x8c.2
#bit  tmr2ie  =0x8c.1
#bit  trm1ie  =0x8c.0

#bit  oscf    =0x8e.3
#bit  not_por =0x8e.1
#bit  not_bod =0x8e.0

#bit  csrc    =0x98.7
#bit  tx9     =0x98.6
#bit  txen    =0x98.5
#bit  sync    =0x98.4
#bit  brgh    =0x98.2
#bit  trmt    =0x98.1
#bit  tx9d    =0x98.0

#bit  wrerr   =0x98.3
#bit  wren    =0x98.2
#bit  wr      =0x98.1
#bit  rd      =0x98.0

#bit  vren    =0x9f.7
#bit  vroe    =0x9f.6
#bit  vrr     =0x9f.5
#bit  vr3     =0x9f.3
#bit  vr2     =0x9f.2
#bit  vr1     =0x9f.1
#bit  vr0     =0x9f.0


// For Assembly Language - Note upper case
// Status Bits
#define IRP 	7
#define RP1 	6
#define RP0 	5
#define NOT_TO 	4
#define NOT_PD 	3
#define Z 	2
#define DC 	1
#define C  	0


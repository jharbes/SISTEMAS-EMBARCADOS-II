// DEFS_18C.H
//
// copyright, Peter H. Anderson, Baltimore, MD, Jan, '02


#define byte int

#nolist

//#define FALSE 0
//#define TRUE !FALSE

#byte TOSU  	= 0xfff
#byte TOSH  	= 0xFFE
#byte TOSL  	= 0xFFD
#byte STKPTR    = 0xFFC
#byte PCLATU    = 0xFFB
#byte PCLATH    = 0xFFA
#byte PCL       = 0xFF9
#byte TBLPTRU   = 0xFF8
#byte TBLPTRH   = 0xFF7
#byte TBLPTRL   = 0xFF6
#byte TABLAT    = 0xFF5
#byte PRODH     = 0xFF4
#byte PRODL     = 0xFF3

#byte INTCON    = 0xFF2
#byte INTCON1   = 0xFF2
#byte INTCON2   = 0xFF1
#byte INTCON3   = 0xFF0

#byte INDF0     = 0xFEF
#byte POSTINC0  = 0xFEE
#byte POSTDEC0  = 0xFED
#byte PREINC0   = 0xFEC
#byte PLUSW0    = 0xFEB
#byte FSR0H     = 0xFEA
#byte FSR0L     = 0xFE9
#byte WREG      = 0xFE8

#byte INDF1     = 0xFE7
#byte POSTINC1  = 0xFE6
#byte POSTDEC1  = 0xFE5
#byte PREINC1   = 0xFE4
#byte PLUSW1    = 0xFE3
#byte FSR1H     = 0xFE2
#byte FSR1L     = 0xFE1
#byte BSR       = 0xFE0

#byte INDF2     = 0xFDF
#byte POSTINC2  = 0xFDE
#byte POSTDEC2  = 0xFDD
#byte PREINC2   = 0xFDC
#byte PLUSW2    = 0xFDB
#byte FSR2H     = 0xFDA
#byte FSR2L     = 0xFD9
#byte STATUS    = 0xFD8

#byte TMR0H     = 0xFD7
#byte TMR0L     = 0xFD6
#byte T0CON     = 0xFD5

// RESERVED_0FD4   = 0xFD4

#byte OSCCON    = 0xFD3
#byte LVDCON    = 0xFD2
#byte WDTCON    = 0xFD1
#byte RCON      = 0xFD0

#byte TMR1H     = 0xFCF
#byte TMR1L     = 0xFCE
#byte T1CON     = 0xFCD
#byte TMR2      = 0xFCC
#byte PR2       = 0xFCB
#byte T2CON     = 0xFCA

#byte SSPBUF    = 0xFC9
#byte SSPADD    = 0xFC8
#byte SSPSTAT   = 0xFC7
#byte SSPCON1   = 0xFC6
#byte SSPCON2   = 0xFC5

#byte ADRESH    = 0xFC4
#byte ADRESL    = 0xFC3
#byte ADCON0    = 0xFC2
#byte ADCON1     = 0xFC1

// RESERVED_0FC0   = 0xFC0

#byte CCPR1H     = 0xFBF
#byte CCPR1L     = 0xFBE
#byte CCP1CON    = 0xFBD
#byte CCPR2H     = 0xFBC
#byte CCPR2L     = 0xFBB
#byte CCP2CON    = 0xFBA

// RESERVED_0FB9   = 0xFB9
// RESERVED_0FB8   = 0xFB8
// RESERVED_0FB7   = 0xFB7
// RESERVED_0FB6   = 0xFB6
// RESERVED_0FB5   = 0xFB5
// RESERVED_0FB4   = 0xFB4

#byte TMR3H      = 0xFB3
#byte TMR3L      = 0xFB2
#byte T3CON      = 0xFB1

// RESERVED_0FB0   = 0xFB0

#byte SPBRG      = 0xFAF
#byte RCREG      = 0xFAE
#byte TXREG      = 0xFAD
#byte TXSTA      = 0xFAC
#byte RCSTA      = 0xFAB

// RESERVED_0FAA   = 0xFAA
// RESERVED_0FA9   = 0xFA9
// RESERVED_0FA8   = 0xFA8
// RESERVED_0FA7   = 0xFA7
// RESERVED_0FA6   = 0xFA6
// RESERVED_0FA5   = 0xFA5
// RESERVED_0FA4   = 0xFA4
// RESERVED_0FA3   = 0xFA3

#byte IPR2       = 0xFA2
#byte PIR2       = 0xFA1
#byte PIE2       = 0xFA0
#byte IPR1       = 0xF9F
#byte PIR1       = 0xF9E
#byte PIE1       = 0xF9D

// RESERVED_0F9C   = 0xF9C
// RESERVED_0F9B   = 0xF9B
// RESERVED_0F9A   = 0xF9A
// RESERVED_0F99   = 0xF99
// RESERVED_0F98   = 0xF98
// RESERVED_0F97   = 0xF97

#byte TRISE      = 0xF96
#byte TRISD      = 0xF95
#byte TRISC      = 0xF94
#byte TRISB      = 0xF93
#byte TRISA      = 0xF92

// RESERVED_0F91   = 0xF91
// RESERVED_0F90   = 0xF90
// RESERVED_0F8F   = 0xF8F
// RESERVED_0F8E   = 0xF8E

#byte LATE       = 0xF8D
#byte LATD       = 0xF8C
#byte LATC       = 0xF8B
#byte LATB       = 0xF8A
#byte LATA       = 0xF89

// RESERVED_0F88   = 0xF88
// RESERVED_0F87   = 0xF87
// RESERVED_0F86   = 0xF86
// RESERVED_0F85   = 0xF85

#byte PORTE      = 0xF84
#byte PORTD      = 0xF83
#byte PORTC      = 0xF82
#byte PORTB      = 0xF81
#byte PORTA      = 0xF80

// ----- STKPTR Bits --------------------------------------------------------

#bit stkful 	= STKPTR.7
#bit stkunf 	= STKPTR.6

// ----- INTCON Bits --------------------------------------------------------

#bit gie		= INTCON.7
#bit gieh		= INTCON.7
#bit peie 		= INTCON.6
#bit giel		= INTCON.6
#bit tmr0ie		= INTCON.5
#bit t0ie		= INTCON.5	// backward compatibility
#bit int0ie		= INTCON.4
#bit int0e		= INTCON.4  // For backward compatibility
#bit rbie		= INTCON.3
#bit tmr0if		= INTCON.2
#bit t0if		= INTCON.2	// For backward compatibility
#bit int0if		= INTCON.1
#bit int0f		= INTCON.1	// For backward compatibility
#bit rbif		= INTCON.0

// ----- INTCON2 Bits --------------------------------------------------------
#bit not_rbpu	= INTCON.7
#bit rbpu		= INTCON.7
#bit intedg0	= INTCON.6
#bit intedg1	= INTCON.5
#bit intedg2	= INTCON.4
#bit tmr0ip		= INTCON.2
#bit t0ip		= INTCON.2	// For compatibility with T0IE and T0IF
#bit rbip		= INTCON.0

// ----- INTCON3 Bits --------------------------------------------------------
#bit int2ip     = INTCON3.7
#bit int1ip     = INTCON3.6
#bit int2ie     = INTCON3.4
#bit int1ie     = INTCON3.3
#bit int2if     = INTCON3.1
#bit int1if     = INTCON3.0

// ----- STATUS Bits --------------------------------------------------------
//N               EQU  H'0004'
//OV              EQU  H'0003'
//Z               EQU  H'0002'
//DC              EQU  H'0001'
//C               EQU  H'0000'

// ----- T0CON Bits ---------------------------------------------------------
#bit tmr0on    = T0CON.7
#bit t08bit    = T0CON.6
#bit t0cs      = T0CON.5
#bit t0se      = T0CON.4
#bit psa       = T0CON.3
#bit t0ps2     = T0CON.2
#bit t0ps1     = T0CON.1
#bit t0ps0     = T0CON.0

// ----- OSCON Bits ---------------------------------------------------------
#bit scs	   = OSCCON.0

// ----- LVDCON Bits ---------------------------------------------------------
#bit irvst     = LVDCON.5
#bit lvden     = LVDCON.4
#bit lvdl3     = LVDCON.3
#bit lvdl2     = LVDCON.2
#bit lvdl1     = LVDCON.1
#bit lvdl0     = LVDCON.0

// ----- WDTCON Bits ---------------------------------------------------------
#bit swdten     = WDTCON.0

// ----- RCON Bits -----------------------------------------------------------
#bit ipen       = RCON.7
#bit lwrt       = RCON.6
#bit not_ri     = RCON.4
// #bit ri      = RCON.4
#bit not_to     = RCON.3
// #bit to      = RCON.3
#bit not_pd     = RCON.2
// #bit pd      = RCON.2
#bit not_por    = RCON.1
// #bit por     = RCON.1
#bit not_bor    = RCON.0
// #bit bor     = RCON.0

// ----- T1CON Bits ---------------------------------------------------------
#bit t1rd16  	= T1CON.7

#bit t1ckps1 	= T1CON.5
#bit t1ckps0 	= T1CON.4
#bit t1oscen 	= T1CON.3
#bit t1sync  	= T1CON.2
#bit tmr1cs  	= T1CON.1
#bit tmr1on  	= T1CON.0


// ----- T2CON Bits ---------------------------------------------------------
#bit toutps3 	= T2CON.6
#bit toutps2 	= T2CON.5
#bit toutps1 	= T2CON.4
#bit toutps0 	= T2CON.3
#bit tmr2on  	= T2CON.2
#bit t2ckps1 	= T2CON.1
#bit t2ckps0 	= T2CON.0

// ----- SSPSTAT Bits -------------------------------------------------------

#bit stat_smp 	= SSPSTAT.7
#bit stat_cke 	= SSPSTAT.6
#bit stat_da 	= SSPSTAT.5
#bit stat_p  	= SSPSTAT.4
#bit stat_s  	= SSPSTAT.3
#bit stat_rw 	= SSPSTAT.2
#bit stat_ua 	= SSPSTAT.1
#bit stat_bf 	= SSPSTAT.0

// ----- SSPCON1 Bits --------------------------------------------------------
#bit wcol       = SSPCON1.7
#bit sspov      = SSPCON1.6
#bit sspen      = SSPCON1.5
#bit ckp        = SSPCON1.4
#bit sspm3      = SSPCON1.3
#bit sspm2      = SSPCON1.2
#bit sspm1      = SSPCON1.1
#bit sspm0      = SSPCON1.0

// ----- SSPCON2 Bits --------------------------------------------------------

#bit gcen 		= SSPCON2.7
#bit ackstat	= SSPCON2.6
#bit ackdt 		= SSPCON2.5
#bit acken 		= SSPCON2.4
#bit rcen 		= SSPCON2.3
#bit pen 		= SSPCON2.2
#bit rsen		= SSPCON2.1
#bit sen 		= SSPCON2.0

// ----- ADCON0 Bits --------------------------------------------------------
#bit adcs1 		= ADCON0.7
#bit adcs0 		= ADCON0.6
#bit chs2  		= ADCON0.5
#bit chs1  		= ADCON0.4
#bit chs0  		= ADCON0.3
#bit adgo  		= ADCON0.2

#bit adon  		= ADCON0.0

// ----- ADCON1 Bits --------------------------------------------------------
#bit adfm   	= ADCON1.7
#bit adcs2  	= ADCON1.6

#bit pcfg3  	= ADCON1.3
#bit pcfg2  	= ADCON1.2
#bit pcfg1  	= ADCON1.1
#bit pcfg0   	= ADCON1.0

// ----- CCP1CON Bits -------------------------------------------------------
#bit dc1b1      = CCP1CON.5
#bit ccp1x      = CCP1CON.5        // for backward compatibility
#bit dc1b0      = CCP1CON.4
#bit ccp1y      = CCP1CON.4        // for backward compatibility
#bit ccp1m3     = CCP1CON.3
#bit ccp1m2     = CCP1CON.2
#bit ccp1m1     = CCP1CON.1
#bit ccp1m0     = CCP1CON.0

// ----- CCP2CON Bits -------------------------------------------------------
#bit dc2b1      = CCP2CON.5
#bit ccp2x      = CCP2CON.5        // for backward compatibility
#bit dc2b0      = CCP2CON.4
#bit ccp2y      = CCP2CON.4        // for backward compatibility
#bit ccp2m3     = CCP2CON.3
#bit ccp2m2     = CCP2CON.2
#bit ccp2m1     = CCP2CON.1
#bit ccp2m0     = CCP2CON.0

// ----- T3CON Bits ---------------------------------------------------------
#bit t3rd16       = T3CON.7
#bit t3ccp2     = T3CON.6
#bit t3ckps1    = T3CON.5
#bit t3ckps0    = T3CON.4
#bit t3ccp1     = T3CON.3
#bit not_t3sync = T3CON.2
#bit t3sync     = T3CON.2
#bit t3insync   = T3CON.2        // backward compatibility only
#bit tmr3cs     = T3CON.1
#bit tmr3on     = T3CON.0

// ----- TXSTA Bits ---------------------------------------------------------

#bit csrc  		= TXSTA.7
#bit tx9  		= TXSTA.6
#bit txen  		= TXSTA.5
#bit sync  		= TXSTA.4

#bit brgh  		= TXSTA.2
#bit trmt  		= TXSTA.1
#bit tx9d  		= TXSTA.0

// ----- RCSTA Bits ---------------------------------------------------------

#bit spen 		= RCSTA.7
#bit rx9  		= RCSTA.6
#bit sren 		= RCSTA.5
#bit cren 		= RCSTA.4
#bit adden		= RCSTA.3
#bit ferr 		= RCSTA.2
#bit oerr 		= RCSTA.1
#bit rx9d 		= RCSTA.0

// ----- IPR2 Bits ----------------------------------------------------------
#bit bclip      = IPR2.3
#bit lvdip      = IPR2.2
#bit tmr3ip     = IPR2.1
#bit ccp2ip     = IPR2.0

// ----- PIR2 Bits ----------------------------------------------------------
#bit bclif      = PIR2.3
#bit lvdif      = PIR2.2
#bit tmr3if     = PIR2.1
#bit ccp2if     = PIR2.0

// ----- PIE2 Bits ----------------------------------------------------------
#bit bclie   	= PIE2.3
#bit lvdie   	= PIE2.2
#bit tmr3ie  	= PIE2.1
#bit ccp2ie  	= PIE2.0

// ----- IPR1 Bits ----------------------------------------------------------
#bit pspip      = IPR1.7
#bit adip       = IPR1.7
#bit rcip       = IPR1.7
#bit txip       = IPR1.7
#bit sspip      = IPR1.7
#bit ccp1ip     = IPR1.7
#bit tmr2ip     = IPR1.7
#bit tmr1ip     = IPR1.7

// ----- PIR1 Bits ----------------------------------------------------------
#bit pspif 		= PIR1.7
#bit adif 		= PIR1.6
#bit rcif 		= PIR1.5
#bit txif 		= PIR1.4
#bit sspif		= PIR1.3
#bit ccp1if	    = PIR1.2
#bit tmr2if     = PIR1.1
#bit tmr1if     = PIR1.0

// ----- PIE1 Bits ----------------------------------------------------------

#bit pspie 		= PIE1.7
#bit adie  		= PIE1.6
#bit rcie  		= PIE1.5
#bit txie  		= PIE1.4
#bit sspie 		= PIE1.3
#bit ccp1ie		= PIE1.2
#bit tmr2ie		= PIE1.1
#bit tmr1ie	    = PIE1.0

// ----- TRISE Bits ---------------------------------------------------------

#bit ibf        = TRISE.7
#bit obf        = TRISE.6
#bit ibov       = TRISE.5
#bit pspmode    = TRISE.4

#bit trise2     = TRISE.2
#bit trise1     = TRISE.1
#bit trise0     = TRISE.0

// ------ TRISD Bits ------------------------------------

#bit trisd7 	= TRISD.7
#bit trisd6 	= TRISD.6
#bit trisd5 	= TRISD.5
#bit trisd4 	= TRISD.4
#bit trisd3 	= TRISD.3
#bit trisd2 	= TRISD.2
#bit trisd1 	= TRISD.1
#bit trisd0 	= TRISD.0

// ------ TRISC Bits -----------------------------------

#bit trisc7 	= TRISC.7
#bit trisc6 	= TRISC.6
#bit trisc5 	= TRISC.5
#bit trisc4 	= TRISC.4
#bit trisc3 	= TRISC.3
#bit trisc2 	= TRISC.2
#bit trisc1 	= TRISC.1
#bit trisc0 	= TRISC.0

// ------ TRISB Bits ----------------------------------

#bit trisb7 	= TRISB.7
#bit trisb6 	= TRISB.6
#bit trisb5 	= TRISB.5
#bit trisb4 	= TRISB.4
#bit trisb3 	= TRISB.3
#bit trisb2 	= TRISB.2
#bit trisb1 	= TRISB.1
#bit trisb0 	= TRISB.0

// ------- TRISA Bits -----------------------------------

#bit trisa6		= TRISA.6
#bit trisa5 	= TRISA.5
#bit trisa4		= TRISA.4
#bit trisa3 	= TRISA.3
#bit trisa2		= TRISA.2
#bit trisa1 	= TRISA.1
#bit trisa0 	= TRISA.0

// ------- PORTE Bits -----------------------------------

#bit porte2 	= PORTE.2
#bit porte1 	= PORTE.1
#bit porte0 	= PORTE.0

// ------- PORTD Bits -----------------------------------

#bit portd7 	= PORTD.7
#bit portd6 	= PORTD.6
#bit portd5 	= PORTD.5
#bit portd4 	= PORTD.4
#bit portd3 	= PORTD.3
#bit portd2 	= PORTD.2
#bit portd1 	= PORTD.1
#bit portd0 	= PORTD.0

// ------- PORTC Bits -----------------------------------

#bit portc7 	= PORTC.7
#bit portc6 	= PORTC.6
#bit portc5 	= PORTC.5
#bit portc4 	= PORTC.4
#bit portc3 	= PORTC.3
#bit portc2 	= PORTC.2
#bit portc1 	= PORTC.1
#bit portc0 	= PORTC.0

// ------- PORTB Bits -----------------------------------

#bit portb7 	= PORTB.7
#bit portb6 	= PORTB.6
#bit portb5 	= PORTB.5
#bit portb4 	= PORTB.4
#bit portb3 	= PORTB.3
#bit portb2 	= PORTB.2
#bit portb1 	= PORTB.1
#bit portb0 	= PORTB.0

// ------- PORTA Bits -----------------------------------

#bit porta6 	= PORTA.6
#bit porta5 	= PORTA.5
#bit porta4 	= PORTA.4
#bit porta3 	= PORTA.3
#bit porta2 	= PORTA.2
#bit porta1 	= PORTA.1
#bit porta0 	= PORTA.0


// ------- LATE Bits -----------------------------------

#bit late2 	= LATE.2
#bit late1 	= LATE.1
#bit late0 	= LATE.0

// ------- LATD Bits -----------------------------------

#bit latd7 	= LATD.7
#bit latd6 	= LATD.6
#bit latd5 	= LATD.5
#bit latd4 	= LATD.4
#bit latd3 	= LATD.3
#bit latd2 	= LATD.2
#bit latd1 	= LATD.1
#bit latd0 	= LATD.0

// ------- LATC Bits -----------------------------------

#bit latc7 	= LATC.7
#bit latc6 	= LATC.6
#bit latc5 	= LATC.5
#bit latc4 	= LATC.4
#bit latc3 	= LATC.3
#bit latc2 	= LATC.2
#bit latc1 	= LATC.1
#bit latc0 	= LATC.0

// ------- LATB Bits -----------------------------------

#bit latb7 	= LATB.7
#bit latb6 	= LATB.6
#bit latb5 	= LATB.5
#bit latb4 	= LATB.4
#bit latb3 	= LATB.3
#bit latb2 	= LATB.2
#bit latb1 	= LATB.1
#bit latb0 	= LATB.0

// ------- LATA Bits -----------------------------------

#bit lata6 	= LATA.6
#bit lata5 	= LATA.5
#bit lata4 	= LATA.4
#bit lata3 	= LATA.3
#bit lata2 	= LATA.2
#bit lata1 	= LATA.1
#bit lata0 	= LATA.0


#list

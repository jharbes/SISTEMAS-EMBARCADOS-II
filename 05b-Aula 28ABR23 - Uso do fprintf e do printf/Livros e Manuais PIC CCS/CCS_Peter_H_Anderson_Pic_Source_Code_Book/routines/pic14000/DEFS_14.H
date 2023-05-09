// DEFS_14.H
//
// copyright, Peter H. Anderson, Elmore, VT, June, '02

#define byte unsigned int


#byte	TMR0	= 0x01
#byte	PCL	    = 0x02
#byte	STATUS	= 0x03
#byte	FSR	    = 0x04
#byte	PORTA	= 0x05
#byte	PORTC	= 0x07
#byte	PORTD	= 0x08
#byte   PCLATH	= 0x0A
#byte	INTCON	= 0x0B
#byte	PIR1	= 0x0C
#byte	ADTMRL	= 0x0E
#byte	ADTMRH	= 0x0F
#byte	I2CBUF	= 0x13
#byte	I2CCON	= 0x14
#byte   ADCAP	= 0x15
#byte	ADCAPL	= 0x15
#byte	ADCAPH	= 0x16
#byte	ADCON0	= 0x1F

#byte	OPTION	= 0x81
#byte	TRISA	= 0x85
#byte	TRISC	= 0x87
#byte	TRISD	= 0x88
#byte 	PIE1	= 0x8C
#byte 	PCON	= 0x8E
#byte 	SLPCON	= 0x8F
#byte 	I2CADD	= 0x93
#byte 	I2CSTAT	= 0x94
#byte 	PREFA	= 0x9B
#byte 	PREFB	= 0x9C
#byte 	CMCON	= 0x9D
#byte 	MISC	= 0x9E
#byte 	ADCON1	= 0x9F

// STATUS bits
#define IRP 8
#define RP1 6
#define RP0     5
#define TO 4
#define PD 3
#define ZERO  2
#define DC    1
#define C   0


//      PORTA bits
#bit porta7		= PORTA.7
#bit porta6		= PORTA.6
#bit porta5		= PORTA.5
#bit porta4		= PORTA.4
#bit porta3		= PORTA.3
#bit porta2		= PORTA.2
#bit porta1		= PORTA.1
#bit porta0		= PORTA.0


//      PORTC bits

#bit portc7		= PORTC.7
#bit portc6		= PORTC.6
#bit portc5		= PORTC.5
#bit portc4		= PORTC.4
#bit portc3		= PORTC.3
#bit portc2		= PORTC.2
#bit portc1		= PORTC.1
#bit portc0		= PORTC.0

//      PORTD bits

#bit portd7		= PORTD.7
#bit portd6		= PORTD.6
#bit portd5		= PORTD.5
#bit portd4		= PORTD.4
#bit portd3		= PORTD.3
#bit portd2		= PORTD.2
#bit portd1		= PORTD.1
#bit portd0		= PORTD.0


//	INTCON bits

#bit gie 		= INTCON.7
#bit peie		= INTCON.6
#bit t0ie		= INTCON.5
#bit t0if	    = INTCON.2


//	PIR1 bits
#bit cmif	= PIR1.7
#bit pbif	= PIR1.4
#bit i2cif	= PIR1.3
#bit rcif	= PIR1.2
#bit adcif	= PIR1.1
#bit ovfif	= PIR1.0

//	I2CCON bits
#bit	wcol	= I2CCON.7
#bit	i2cov	= I2CCON.6
#bit	i2cen	= I2CCON.5
#bit	ckp		= I2CCON.4
#bit	i2cm3	= I2CCON.3
#bit	i2cm2	= I2CCON.2
#bit	i2cm1	= I2CCON.1
#bit	i2cm0	= I2CCON.0

//	ADCON0 bits
#bit	adcs3	= ADCON0.7
#bit	adcs2	= ADCON0.6
#bit	adcs1	= ADCON0.5
#bit	adcs0	= ADCON0.4
#bit	amuxoe	= ADCON0.2
#bit	adrst	= ADCON0.1
#bit	adzero	= ADCON0.0

//	OPTION bits
#bit	rcpu	= OPTION.7
#bit	t0cs	= OPTION.5
#bit	t0se	= OPTION.4
#bit	psa		= OPTION.3
#bit	ps2		= OPTION.2
#bit	ps1		= OPTION.1
#bit	ps0		= OPTION.0

//      TRISA bits
#bit trisa7 	= TRISA.7
#bit trisa6 	= TRISA.6
#bit trisa5 	= TRISA.5
#bit trisa4 	= TRISA.4
#bit trisa3 	= TRISA.3
#bit trisa2 	= TRISA.2
#bit trisa1 	= TRISA.1
#bit trisa0 	= TRISA.0

//      TRISC bits
#bit trisc7 	= TRISC.7
#bit trisc6 	= TRISC.6
#bit trisc5 	= TRISC.5
#bit trisc4 	= TRISC.4
#bit trisc3 	= TRISC.3
#bit trisc2 	= TRISC.2
#bit trisc1 	= TRISC.1
#bit trisc0 	= TRISC.0

//     TRISD bits
#bit trisd7 	= TRISD.7
#bit trisd6 	= TRISD.6
#bit trisd5 	= TRISD.5
#bit trisd4 	= TRISD.4
#bit trisd3 	= TRISD.3
#bit trisd2 	= TRISD.2
#bit trisd1 	= TRISD.1
#bit trisd0 	= TRISD.0

//	PIE1 bits
#bit cmie 	= PIE1.7
#bit pbie	= PIE1.4
#bit i2cie	= PIE1.3
#bit rcie	= PIE1.2
#bit adcie	= PIE1.1
#bit ovfie	= PIE1.0

//	PCON bits
#bit por	= PCON.1
#bit lvd	= PCON.0

//	SLPCON bits
#bit hiben	= SLPCON.7
#bit refoff	= SLPCON.5
#bit lsoff	= SLPCON.4
#bit oscoff	= SLPCON.3
#bit cmoff	= SLPCON.2
#bit tempoff= SLPCON.1
#bit adoff	= SLPCON.0

//	I2CSTAT bits
#bit stat_da	= I2CSTAT.5
#bit stat_p		= I2CSTAT.4
#bit stat_s		= I2CSTAT.3
#bit stat_rw	= I2CSTAT.2
#bit stat_ua	= I2CSTAT.1
#bit stat_bf	= I2CSTAT.0

/*	PREFA bits	*/
#bit	pra7	= PREFA.7
#bit	pra6	= PREFA.6
#bit	pra5	= PREFA.5
#bit	pra4	= PREFA.4
#bit	pra3	= PREFA.3
#bit	pra2	= PREFA.2
#bit	pra1	= PREFA.1
#bit	pra0	= PREFA.0

/*	PREFB bits	*/
#bit	prb7	= PREFB.7
#bit	prb6	= PREFB.6
#bit	prb5	= PREFB.5
#bit	prb4	= PREFB.4
#bit	prb3	= PREFB.3
#bit	prb2	= PREFB.2
#bit	prb1	= PREFB.1
#bit	prb0	= PREFB.0

/*	CMCON bits	*/
#bit	cmbout	= CMCON.6
#bit	cmboe	= CMCON.5
#bit	cpolb	= CMCON.4
#bit	cmaout	= CMCON.2
#bit	cmaoe	= CMCON.1
#bit	cpola	= CMCON.0

/*	MISC bits	*/
#bit	smhog	= MISC.7
#bit	spgndb	= MISC.6
#bit	spgnda	= MISC.5
#bit	i2csel	= MISC.4
#bit	smbus	= MISC.3
#bit	inclken	= MISC.2
#bit	osc2	= MISC.1
#bit	osc1	= MISC.0

/*	ADCON1 bits	*/
#bit	addac3	= ADCON1.7
#bit	addac2	= ADCON1.6
#bit	addac1	= ADCON1.5
#bit	addac0	= ADCON1.4
#bit	pcfg3	= ADCON1.3
#bit	pcfg2	= ADCON1.2
#bit	pcfg1	= ADCON1.1
#bit	pcfg0	= ADCON1.0

#ifdef JUNK

#define CONFIG_ADDR	0x2007
#define FOSC		0x01
#define WDTE		0x04
#define PWRTE		0x08
#define CPC		0x80

#define CPP0		0x10
#define CPP1		0x20

#define UNPROTECT	(CPP0 | CPP1)
#define PROTECT 	0x00

/*      ADC calibration data etc. Access with get_cal_data, e.g.
 *
 *      x = get_cal_data(KREF);
 *
 */

extern double get_cal_data(const unsigned char *);

static const unsigned char      KREF[4]    @ 0xFC0;     /* Slope reference ratio */
static const unsigned char      KBG[4]     @ 0xFC4;     /* Bandgap reference voltage */
static const unsigned char      VTHERM[4]  @ 0xFC8;     /* Temperature sensor voltage */
static const unsigned char      KTC[4]     @ 0xFCC;     /* Temperature sensor coefficient */
static const unsigned char      OSCM       @ 0xFD0;     /* Oscillator freq. multiplier */
static const unsigned char      TWDT       @ 0xFD2;     /* WDT time-out */

#endif

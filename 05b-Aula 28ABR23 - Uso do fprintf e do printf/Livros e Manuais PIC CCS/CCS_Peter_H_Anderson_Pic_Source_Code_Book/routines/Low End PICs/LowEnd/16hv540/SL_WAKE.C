// SL_WAKE.C (PIC16HV540), CCS-PCB
//
// Illustrates use of wakeup on slow change on PORTB7 and determining the cause of
// a reset.
//
// Capacitor is discharged and a task is performed.  PORTB7 is then brought to a high
// impedance and the processor goes into the "sleep" mode.  Processor wakes up when the
// capacitor charges to some threshold.  The idea is an inexpensive alternative to using
// the watch dog timer.
//
// On boot, the program reads the /PCWUF, /TO and /PD bits and identifies if the cause
// of the interrupt is "power on", "/MCLR from sleep", "wake-up from sleep" or other and
// flashes the appropraite LED and enters the sleep mode.
//
//
//                                     +12
//                                       |
//                                      100K
// PORTB7 (term 13) ------- 1K --------- | +
//                                       -
//                                       - 47 uFd
//                                       |
//                                      GRD
// PORTB0 - 3 are tied to ground
//                                  LED
// PORTA3 (term 2) ---------- 330 ---->|--------- GRD (Unknown reset cause)
// PORTA2 (term 1) ---------- 330 ---->|--------- GRD (Wakeup on change)
// PORTA1 (term 18) --------- 330 ---->|--------- GRD (/MCLR wakeup from sleep)
// PORTA0 (term 17) --------- 330 ---->|--------- GRD (power on reset)
//
// copyright, Peter H. Anderson, Elmore, VT, July, '01

#case

#device PIC16HV540 *=8

#include <defs_540.h>
#include <delay.h>

void config_processor(void);

void do_power_on_task(void);
void do_mclr_from_sleep_task(void);
void do_wakeup_pin_change_task(void);
void do_unknown_reset_task(void);

void main(void)
{
    byte n;
    DIRA = 0x0f;
    DIRB = 0xff;
    config_processor();
    if (not_pcwuf && not_to && not_pd)
    {
       // power on reset
       portb7 = 0;  // ground external RC network
       dirb7 = 0;
#asm
       MOVF DIRB, W
       TRIS PORTB
#endasm
       do_power_on_task();

       dirb7 = 1;  // remove ground on external RC
#asm
       MOVF DIRB, W
       TRIS PORTB
#endasm
#asm
       SLEEP
#endasm
    }

    else if (!not_pcwuf && not_to && !not_pd) // entered sleep mode and pin change flag is at zero
    {
       // wakeup on PORTB
       portb7 = 0;  // ground external RC network
       dirb7 = 0;
 #asm
       MOVF DIRB, W
       TRIS PORTB
 #endasm
       do_wakeup_pin_change_task();

       dirb7 = 1;  // remove ground on external RC
 #asm
       MOVF DIRB, W
       TRIS PORTB
 #endasm
 #asm
       SLEEP
 #endasm
    }

    else if (not_pcwuf && not_to && !not_pd)
    {
        // MCLR wakeup from sleep
        portb7 = 0;  // ground external RC network
        dirb7 = 0;
#asm
        MOVF DIRB, W
        TRIS PORTB
#endasm
        do_mclr_from_sleep_task();
        dirb7 = 1;  // remove ground on external RC
#asm
        MOVF DIRB, W
        TRIS PORTB
#endasm
#asm
        SLEEP
#endasm
    }

    else
    {
        portb7 = 0;  // ground external RC network
        dirb7 = 0;
#asm
        MOVF DIRB, W
        TRIS PORTB
#endasm
        do_unknown_reset_task();

        dirb7 = 1;  // remove ground on external RC
#asm
        MOVF DIRB, W
        TRIS PORTB
#endasm
#asm
  SLEEP
#endasm
    }
}

void do_power_on_task(void)
{
    byte n;
    porta0 = 0;
    dira0 = 0;
#asm
    MOVF DIRA, W
    TRIS PORTA
#endasm
    for (n = 0; n<10; n++)
    {
       porta0 = 1;
       delay_ms(100);
       porta0 = 0;
       delay_ms(100);
    }
}

void do_mclr_from_sleep_task(void)
{
    byte n;
    porta1 = 0;
    dira1 = 0;
#asm
    MOVF DIRA, W
    TRIS PORTA
#endasm
    for (n = 0; n<10; n++)
    {
       porta1 = 1;
       delay_ms(100);
       porta1 = 0;
       delay_ms(100);
    }
}


void do_wakeup_pin_change_task(void)
{
    byte n;
    porta2 = 0;
    dira2 = 0; // make an output
#asm
    MOVF DIRA, W
    TRIS PORTA
#endasm

    for (n=0; n<10; n++)
    {
       porta2 = 1;
       delay_ms(100);
       porta2 = 0;
       delay_ms(100);
    }
}

void do_unknown_reset_task(void)
{
    byte n;
    porta3 = 0;
    dira3 = 0;
#asm
    MOVF DIRA, W
    TRIS PORTA
#endasm
    for (n = 0; n<10; n++)
    {
       porta3 = 1;
       delay_ms(100);
       porta3 = 0;
       delay_ms(100);
    }
}

void config_processor(void) // configure OPTION2 registers
{
    not_pcwu = 0; // wakeup enabled
    not_swdten = 1;
    rl = 1;   // regulated voltage is 5V
    sl = 1;   // sleep level same as RL
    not_boden = 1; // brownout disabled
#asm
    MOVF OPTIONS2, W
    TRIS 0x07
#endasm
}

#include <delay.c>

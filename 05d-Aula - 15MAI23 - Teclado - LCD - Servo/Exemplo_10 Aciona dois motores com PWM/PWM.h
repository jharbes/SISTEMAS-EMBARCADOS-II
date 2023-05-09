#include <18F4520.h>
//#device ADC=10

#FUSES NOWDT  // sem Watch Dog Timer
#FUSES MCLR   // com Master Clear

#use delay(internal=8000000,restart_wdt)
#USE PWM(TIMER=2, OUTPUT=PIN_C2, FREQUENCY=1000Hz, DUTY=50, STREAM=MOTOR_1)
#USE PWM(TIMER=1, OUTPUT=PIN_C3, FREQUENCY=1000Hz, DUTY=50, STREAM=MOTOR_2)


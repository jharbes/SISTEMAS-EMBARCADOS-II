#include <18F4520.h>
#device ADC=10

#FUSES NOWDT                 	//No Watch Dog Timer

#use delay(internal=8MHz,restart_wdt)

#define LED PIN_B0
#define DELAY 1000



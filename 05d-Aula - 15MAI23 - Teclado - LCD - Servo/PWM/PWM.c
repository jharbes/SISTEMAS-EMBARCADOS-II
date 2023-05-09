#include <PWM.h>
int16 i;

void main()
{
	setup_adc_ports(NO_ANALOGS, VSS_VDD);

	while(TRUE)
	{

	pwm_off(MOTOR_2);// começa com  o MOTOR_2 desligado
 	
	pwm_on(MOTOR_1);// começa com  o MOTOR_1 ligado
 	
	// varia do duty cycle de MOTOR_1 de 10% (100) a 90% (900) a cada 0.5s
	for(i=1; i< 10; i++){
	pwm_set_duty_percent(MOTOR_1,i*100);
	delay_ms(500);
	}
	pwm_off(MOTOR_1);
 	
	delay_ms(500);
 	
	// varia do duty cycle de MOTOR_1 de 10% (100) a 90% (900) a cada 0.5s
	pwm_on(MOTOR_2);
	for(i=1; i< 10; i++){
	pwm_set_duty_percent(MOTOR_2,i*100);
	delay_ms(500);
	}

	pwm_off(MOTOR_2);
	 	
	}

}

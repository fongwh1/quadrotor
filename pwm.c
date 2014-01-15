#include "module_motor.h"
#include "stm32f4xx.h"
#include "stm32f4_delay.h"
#include "string-util.h"

void PWM_Config( void ){

	Motor_Config();

}

void PWM_print()
{
        printf("1: %d 2: %d 3: %d 4: %d\n",PWM_Motor1,PWM_Motor2,PWM_Motor3,PWM_Motor4);
}

void PWM_test(void){

	u16	PWM_pulse = PWM_MOTOR_MIN;
	u8 	count;
	while( 1 ){
		while(PWM_pulse < PWM_MOTOR_MAX - 200)
		{
			PWM_pulse += 10 ;

			PWM_Motor1 = PWM_pulse;
  			PWM_Motor2 = PWM_pulse;
  			PWM_Motor3 = PWM_pulse;
  			PWM_Motor4 = PWM_pulse;
			
			if ((PWM_pulse % 100) == 0 )
			{
				PWM_print();
			}

			Delay_100ms(1);
		}

		while(PWM_pulse > PWM_MOTOR_MIN)
		{
			PWM_pulse -= 10;

			PWM_Motor1 = PWM_pulse;
                        PWM_Motor2 = PWM_pulse;
                        PWM_Motor3 = PWM_pulse;
                        PWM_Motor4 = PWM_pulse;

			if ((PWM_pulse % 100) == 0 )
                        {
                                PWM_print();
                        }


                        Delay_100ms(1);
		}		

	}

}


void PWM_task(void * pvParameters)
{
	
	PWM_test();

	while(1)
	{
	}

}

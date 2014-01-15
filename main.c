/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"



#include "stm32f4_system.h"
#include "stm32f4_delay.h"
#include "QCopterFC.h"
#include "QCopterFC_ctrl.h"
#include "QCopterFC_ahrs.h"
#include "QCopterFC_transport.h"
#include "module_board.h"
#include "module_rs232.h"
#include "module_motor.h"
#include "module_sensor.h"
#include "module_nrf24l01.h"

int main( void )
{
  u8 Sta = ERROR;
  FSM_Mode FSM_State = FSM_Rx;

  /* System Init */
  System_Init();



  /* Lock */
  LED_R = 1;
  LED_G = 0;
  LED_B = 1;

  /* Final State Machine */
  while(1) {
	LED_G = ~LED_G; 
	LED_B = ~LED_B;
	LED_R = ~LED_R;

	Delay_100ms(10);	

  }
	return 0;
}
/*=====================================================================================================*/
/*=====================================================================================================*/

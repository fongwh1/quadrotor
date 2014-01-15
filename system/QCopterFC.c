/*=====================================================================================================*/
/*=====================================================================================================*/
#include "stm32f4_system.h"
#include "QCopterFC.h"
#include "QCopterFC_ctrl.h"
#include "QCopterFC_ahrs.h"
#include "QCopterFC_transport.h"
#include "module_board.h"
#include "module_rs232.h"
#include "module_motor.h"
#include "module_sensor.h"
#include "module_nrf24l01.h"
/*=====================================================================================================*/
/*=====================================================================================================*/
void System_Init( void )
{
  SystemInit();

  LED_Config();
  KEY_Config();
  RS232_Config();
  Motor_Config();
  Sensor_Config();
  nRF24L01_Config();

  PID_Init(&PID_Yaw);
  PID_Init(&PID_Roll);
  PID_Init(&PID_Pitch);

  PID_Pitch.Kp = +1.5f;
  PID_Pitch.Ki = +0.002f;
  PID_Pitch.Kd = +1.0f;

  PID_Roll.Kp  = +1.5f;
  PID_Roll.Ki  = +0.002f;
  PID_Roll.Kd  = +1.0f;

  PID_Yaw.Kp   = +0.0f;
  PID_Yaw.Ki   = +0.0f;
  PID_Yaw.Kd   = +0.0f;

  Delay_10ms(2);
}
/*=====================================================================================================*/
/*=====================================================================================================*/


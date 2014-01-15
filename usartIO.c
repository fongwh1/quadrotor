#include "stm32f4_system.h"
#include "stm32f4_delay.h"
#include "stm32f4_usart.h"
#include "module_rs232.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

volatile xSemaphoreHandle serial_tx_wait_sem = NULL;

volatile xQueueHandle serial_rx_queue = NULL;

typedef struct {
	char ch;
}serial_ch_msg;

void USARTIO_Init(){
	/*Create the queue used by the serial task.
	 *Messages for write to the RS232*/
	vSemaphoreCreateBinary(serial_tx_wait_sem);
	serial_rx_queue = xQueueCreate(1, sizeof(serial_ch_msg));
}

void enable_rs232_interrupts(void){
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/*Enable transmit and receive interrupts for the USART3*/
	USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

	/*Enable the USART3 IRQ in the NVIC module
	 *(so that the USART3 interrupt handler is enabled)*/
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/*IRQ handler to handle USART3 interrupts(both transmit and receive interrupts)*/
void USART3_IRQHandler(){
	static signed portBASE_TYPE xHigherPriorityTaskWoken;
	serial_ch_msg rx_msg;

	/*If this interrupt is for a transmit...*/
	if(USART_GetITStatus(USART3, USART_IT_TXE) != RESET){
		/*"give" the serial_tx_wait_sem semaphore to notify processes
		 *that the buffer has a spot free for the next byte.
		 */
		xSemaphoreGiveFromISR(serial_tx_wait_sem, &xHigherPriorityTaskWoken);
		/*Disable the transmit interrupt.*/
		USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
	/*If this interrupt is for a receive...*/
	}else if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET){
		/*Receive the byte from the buffer.*/
		rx_msg.ch = USART_RecvByte(USART3);

		/*Queue the received byte.*/
		if(!xQueueSendToBackFromISR(serial_rx_queue, &rx_msg, &xHigherPriorityTaskWoken)){
			/*If there was an error queueing the received byte, freeze!*/
			while(1);
		}
	/*Only transmit and receive interrupts should be enabled.
	 *If this is another type of interrupt, freeze.*/
	}else{
		while(1);
	}

	if(xHigherPriorityTaskWoken){
		taskYIELD();
	}
}

void send_byte(char ch){
	/*Wait until the RS232 port can receive another byte
	 *(this semaphore is "given" by the RS232 port interrupt
	 *when the buffer has room for another byte.)
	 */
	while (!xSemaphoreTake(serial_tx_wait_sem, portMAX_DELAY));
	/*Send the byte and enable the transmit interrupt
	 *(it is diabled by the interrupt).
	 */
	USART_SendData(USART3, ch);
	USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
}

char receive_byte(){
	serial_ch_msg msg;
	/*Wait for a byte to be queued by the receive interrupts handler.*/
	while(!xQueueReceive(serial_rx_queue, &msg, portMAX_DELAY));
	return msg.ch;
}

char non_block_receive_byte(){
	serial_ch_msg msg;
	
	int ret = xQueueReceive(serial_rx_queue, &msg, portMAX_DELAY);
	if(ret == pdTRUE){
		return msg.ch;
	}
	return '\0';
}

void send_str(char *str){
	while (!xSemaphoreTake(serial_tx_wait_sem, portMAX_DELAY));
	RS232_SendStr(USART3, (uc8 *)str);
	USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
}

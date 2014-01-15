#ifndef __USARTIO_H
#define __USARTIO_H

void USARTIO_init(void);
void enable_rs232_interrupts(void);
void USART_IRQHandler(void);
void send_byte(char ch);
char receive_byte(void);
char non_block_receive_byte(void);
void send_str(char *str);

#endif

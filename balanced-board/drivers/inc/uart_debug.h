#ifndef UART_DEBUG_H_
#define UART_DEBUG_H_

#include <stm32f3xx.h>
#include <threads.h>

#define START_FLAG 0x12
#define STOP_FLAG 0x13
#define ESCAPE_FLAG 0x7d

void uart_debug_init(UART_HandleTypeDef *);
void uart_debug_sendStream(char *, size_t);
void uart_debug_sendString(char *buff_ptr, size_t len);
#endif /* UART_DEBUG_H_ */

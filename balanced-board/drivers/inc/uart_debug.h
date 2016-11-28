#ifndef UART_DEBUG_H_
#define UART_DEBUG_H_

#include <stm32f3xx.h>

void uart_debug_init(UART_HandleTypeDef *huart);
void uart_debug_sendString(char *c);

#endif /* UART_DEBUG_H */

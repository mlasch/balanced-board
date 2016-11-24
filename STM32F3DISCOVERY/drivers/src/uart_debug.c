#include <stm32f3xx.h>

#include <uart_debug.h>
#include <globals.h>

#include <string.h>

UART_HandleTypeDef UART4_Handle;

void uart_debug_init(UART_HandleTypeDef *huart)
{
	// 1N8 9600 baud
	
	huart->Instance = UART4;
	huart->gState = HAL_UART_STATE_RESET;
	huart->Init.BaudRate = 115200;
	huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart->Init.Mode = UART_MODE_TX_RX;
	huart->Init.OneBitSampling = UART_ONEBIT_SAMPLING_DISABLED;
	huart->Init.OverSampling = UART_OVERSAMPLING_16;		// Oversampling for clock regeneration
	huart->Init.Parity = UART_PARITY_NONE;
	huart->Init.StopBits = UART_STOPBITS_1;
	huart->Init.WordLength = UART_WORDLENGTH_8B;
	
	if (HAL_UART_Init(huart) != HAL_OK) {
		for (;;);
	}
}

void uart_debug_sendString(char *c) {
	HAL_UART_Transmit(&UART4_Handle, (uint8_t *) c, strlen(c), 5000);
}

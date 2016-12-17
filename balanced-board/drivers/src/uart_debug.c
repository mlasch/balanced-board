#include <stm32f3xx.h>

#include <threads.h>
#include <uart_debug.h>
#include <globals.h>

#include <string.h>

UART_HandleTypeDef UART4_Handle;

/* private functions */
static size_t byte_stuffing(char *raw, size_t len, char *stuffed);

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

void uart_debug_sendStream(char *buff_ptr, size_t len) {
	size_t stuffed_len;
	char stuff_buff[64];
	
	stuffed_len = byte_stuffing(buff_ptr, len, stuff_buff);
	
	HAL_UART_Transmit(&UART4_Handle, (uint8_t *) stuff_buff, stuffed_len, 5000);
}

void uart_debug_sendString(char *buff_ptr, size_t len) {
	
	HAL_UART_Transmit(&UART4_Handle, (uint8_t *) buff_ptr, len, 5000);
}

static size_t byte_stuffing(char *raw, size_t len, char *stuffed) {
	int i, j = 0;
	
	stuffed[j++] = START_FLAG;
	
	for(i=0;i<len;i++) {
		switch(raw[i]) {
			case START_FLAG:
			case STOP_FLAG:
			case ESCAPE_FLAG:
				stuffed[j++] = ESCAPE_FLAG;
				stuffed[j++] = raw[i];
				break;
			default:
				stuffed[j++] = raw[i];
				break;
		}
	}
	
	stuffed[j++] = STOP_FLAG;
	
	return j;
}

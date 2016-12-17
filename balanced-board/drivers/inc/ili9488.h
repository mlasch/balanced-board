#ifndef __ILI9488_H
#define __ILI9488_H

/*
 * ILI9488 480x320
 * DBI Type B 8-bit
 */

#include <stm32f3xx.h>

typedef enum {
	io_in,
	io_out
} io_dir_t;

/* pin definitions */
#define PIN_D0			GPIO_PIN_0
#define PIN_D1			GPIO_PIN_1
#define PIN_D2			GPIO_PIN_2
#define PIN_D3			GPIO_PIN_3
#define PIN_D4			GPIO_PIN_4
#define PIN_D5			GPIO_PIN_5
#define PIN_D6			GPIO_PIN_6
#define PIN_D7			GPIO_PIN_7

#define PIN_WRX			GPIO_PIN_8		// write
#define PIN_RDX			GPIO_PIN_9		// read
#define PIN_D_CX		GPIO_PIN_10		// data/command
#define PIN_RESX		GPIO_PIN_11		// reset
#define PIN_CSX			GPIO_PIN_13		// chip select

#define LCD_PORT GPIOD

#define LCD_GPIO_CLOCK_ENABLE() __HAL_RCC_GPIOD_CLK_ENABLE()

/* parallel out with low footprint */
#define LOW_BYTE_TO_BSRR(X) (uint16_t)( X & 0x000000ff) | \
														((uint16_t)(~X & 0x000000ff) << 16)

#define WRX_IDLE		LCD_PORT->BSRR = PIN_WRX
#define WRX_ACTIVE	LCD_PORT->BRR = PIN_WRX
#define RDX_IDLE		LCD_PORT->BSRR = PIN_RDX
#define RDX_ACTIVE	LCD_PORT->BRR = PIN_RDX
#define D_CX_DATA		LCD_PORT->BSRR = PIN_D_CX
#define D_CX_CMD		LCD_PORT->BRR = PIN_D_CX
#define RESX_IDLE		LCD_PORT->BSRR = PIN_RESX
#define RESX_ACTIVE	LCD_PORT->BRR = PIN_RESX
#define CSX_IDLE		LCD_PORT->BSRR = PIN_CSX
#define CSX_ACTIVE	LCD_PORT->BRR = PIN_CSX

#define NOP						0x00
#define SOFT_RESET		0x01
#define READ_DISPLAY_IDENTIFICATION_INFORMATION	0x04
#define SLEEP_OUT 		0x11
#define DISPLAY_OFF		0x28
#define DISPLAY_ON		0x29
#define MEMORY_ACCESS_CONTROL 0x36

#define POWER_CONTROL_1 0xc0
#define POWER_CONTROL_2 0xc1
#define VCOM_CONTROL		0xc5
#define INTERFACE_MODE_CONTROL 0xb0
#define FRAME_RATE_CONTROL 0xb1
#define DISPLAY_INVERSION_CONTROL 0xb4
#define DISPLAY_FUNCTION_CONTROL 0xb6
#define ENTRY_MODE_SET 0xb7
#define INTERFACE_PIXEL_FORMAT 0x3a
#define ADJUST_CONTROL_3 0xF7

void ili9488_init(void);

#endif /* __ILI9488_H */

#include <stm32f3xx.h>
#include <cmsis_os.h>

#include <ili9488.h>

/* configuration for ili9488 display controller */
static const uint8_t ili9488_config[] = {
	POWER_CONTROL_1,				2, 0x10, 0x10,							// Power Control 1 [0e 0e] ???
	POWER_CONTROL_2,				1, 0x41,										// Power Control 2 [43]	???
	VCOM_CONTROL,						4, 0x00, 0x22, 0x80, 0x40,	// VCOM  Control 1 [00 40 00 40] ???
	MEMORY_ACCESS_CONTROL,	1, 0x68,										// Memory Access [00]
	INTERFACE_MODE_CONTROL,	1, 0x00,										// Interface [00]
	FRAME_RATE_CONTROL, 		2, 0xb0, 0x11,							// Frame Rate Control [b0 11]
	DISPLAY_INVERSION_CONTROL,	1, 0x02,								// Inversion Control [02]
	DISPLAY_FUNCTION_CONTROL, 	3, 0x02, 0x02, 0x3b,		// Display Function Control [02 02 3b] .kbv NL=480
	ENTRY_MODE_SET,					1, 0xc6,										// Entry Mode [06]
	INTERFACE_PIXEL_FORMAT, 1, 0x55,										// Interlace Pixel Format [XX]
	ADJUST_CONTROL_3, 4, 0xa9, 0x51, 0x2c, 0x82,				// Adjustment Control 3 [a9 51 2c 82]
	SLEEP_OUT, 0,																				// Sleep Out
};

/* private functions */
static void ili9488_gpio_init(void);
static void ili9488_data_dir(io_dir_t);
static void ili9488_write(uint8_t cmd, uint8_t *data, size_t len);
static void ili9488_read(uint8_t cmd, uint8_t *buffer, size_t len);

static void ili9488_nop(void) __attribute__((unused));
static void ili9488_soft_reset(void) __attribute__((unused));
static void ili9488_display_off(void) __attribute__((unused));
static void ili9488_display_on(void) __attribute__((unused));

void ili9488_init(void) {
	uint8_t *config_ptr = (uint8_t *) ili9488_config;
	size_t config_size = sizeof(ili9488_config);
	
	ili9488_gpio_init();
	
	osDelay(50);
	RESX_ACTIVE;
	osDelay(100);
	RESX_IDLE;
	osDelay(100);
	
	ili9488_soft_reset();
	
	osDelay(50);
	
	ili9488_display_off();
	
	/* write config to controller */
	while ((ili9488_config + config_size) != config_ptr) {
		ili9488_write(*config_ptr, config_ptr+2, *(config_ptr+1));
		config_ptr += (*(config_ptr+1) + 2);
	}
	
	osDelay(150);
	ili9488_display_on();
}

static void ili9488_gpio_init(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	
	LCD_GPIO_CLOCK_ENABLE();
	
	GPIO_InitStructure.Pin = (PIN_WRX | PIN_RDX | PIN_D_CX | PIN_RESX | PIN_CSX);
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	
	HAL_GPIO_Init(LCD_PORT, &GPIO_InitStructure);
	
	/* set the GPIO on startup */
	WRX_IDLE;			// HIGH on idle
	RDX_IDLE;			// HIGH on idle
	D_CX_DATA;		// 0=CMD, 1=DATA
	RESX_IDLE;		// active on LOW
	CSX_IDLE;			// active on LOW
	
	GPIO_InitStructure.Pin = (PIN_D0 | PIN_D1 | PIN_D2 | PIN_D3 | \
														PIN_D4 | PIN_D5 | PIN_D6 | PIN_D7);
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(LCD_PORT, &GPIO_InitStructure);
	
	/* initialize data pins with zeros */
	LCD_PORT->BSRR = LOW_BYTE_TO_BSRR(0x00);
}

static void ili9488_data_dir(io_dir_t io_dir) {
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.Pin = (PIN_D0 | PIN_D1 | PIN_D2 | PIN_D3 | \
														PIN_D4 | PIN_D5 | PIN_D6 | PIN_D7);
	
	if (io_dir == io_out) {
		GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	} else {
		GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	}
	
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(LCD_PORT, &GPIO_InitStructure);
}

static void ili9488_write(uint8_t cmd, uint8_t *data, size_t len) {
	D_CX_CMD;			// 0 = command
	WRX_ACTIVE;
	LCD_PORT->BSRR = LOW_BYTE_TO_BSRR(cmd);
	WRX_IDLE;
	osDelay(1);		// TODO get the right timing
	D_CX_DATA;
	
	while (len) {
		WRX_ACTIVE;
		LCD_PORT->BSRR = LOW_BYTE_TO_BSRR(*data);
		data++;
		WRX_IDLE;
		len--;
	}
}

static void ili9488_read(uint8_t cmd, uint8_t *buffer, size_t len) {
	D_CX_CMD;			// 0 = command
	RDX_ACTIVE;
	LCD_PORT->BSRR = LOW_BYTE_TO_BSRR(cmd);
	RDX_IDLE;
	D_CX_DATA;
	
	ili9488_data_dir(io_in);
	
	while (len+1) {
		// len+1 because of DUMMY data read
		RDX_ACTIVE;
		RDX_IDLE;
		*buffer++ = LCD_PORT->IDR & 0xff;
		len--;
	}
	
	ili9488_data_dir(io_out);
}

static void ili9488_nop(void) {
	CSX_ACTIVE;
	ili9488_write(NOP, NULL, 0);
	CSX_IDLE;
}

static void ili9488_soft_reset(void) {
	CSX_ACTIVE;
	ili9488_write(SOFT_RESET, NULL, 0);
	CSX_IDLE;
}

static void ili9488_display_off(void) {
	CSX_ACTIVE;
	ili9488_write(DISPLAY_OFF, NULL, 0);
	CSX_IDLE;
}

static void ili9488_display_on(void) {
	CSX_ACTIVE;
	ili9488_write(DISPLAY_ON, NULL, 0);
	CSX_IDLE;
}

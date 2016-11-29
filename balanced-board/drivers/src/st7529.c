#include <stm32f3xx.h>
#include <cmsis_os.h>
#include <st7529.h>

/* private functions */
static void st7529_send_command(char);
static void st7529_send_data(char);
static void st7529_set_line(uint32_t line0, uint32_t line1);
static void st7529_set_column(int col0, int col1);
static void LCD_Set_Addr(int Col0, int Col1, int Line0, int Line1);

void HALL_Delay(volatile uint32_t a) {
	int i;
	
	for (i = 0; i < 2000000;i++);
}

void lcdDrawPixel() {
	
}

void st7529_init() {
	HAL_GPIO_WritePin(GPIOD, PIN_XCS, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, PIN_E_RD, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, PIN_WR_RW, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, PIN_A0, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, PIN_RST, GPIO_PIN_RESET);
	HALL_Delay(120);
	HAL_GPIO_WritePin(GPIOD, PIN_RST, GPIO_PIN_SET);
	HALL_Delay(120);
	
	LCD_Set_Addr(0, 79, 0, 127);
	
	st7529_send_command(RAMWR);
	st7529_send_data(0x01);
	st7529_send_data(0x02);
	st7529_send_data(0x04);
	st7529_send_data(0x08);
	st7529_send_data(0x0f);
	st7529_send_data(0x1f);
	st7529_send_data(0x2f);
	st7529_send_data(0x4f);
	st7529_send_data(0x8f);
	st7529_send_data(0xff);
	
	st7529_send_command(EXTIN);
	st7529_send_command(SLPOUT);
	st7529_send_command(OSCON);
	st7529_send_command(PWRCTRL);
	st7529_send_data(0x08);
	HALL_Delay(100);
	st7529_send_command(PWRCTRL);
	st7529_send_data(0x0a);
	st7529_send_command(PWRCTRL);
	st7529_send_data(0x0b);
	st7529_send_command(VOLCTRL);
	st7529_send_data(0x04);
	st7529_send_data(0x04);
	st7529_send_command(DISCTRL);
	st7529_send_data(0x00);
	st7529_send_data(0x23);
	st7529_send_data(0x00);
	st7529_send_command(DISNOR);			// normal display
  st7529_send_command(COMSCN);			// COM Scan Direction 
	st7529_send_data(0x02);
	st7529_send_command(DATSDR);			// Data Scan Direction 
	st7529_send_data(0x03); 
	st7529_send_data(0x01);
	st7529_send_data(0x02);
	st7529_send_command(CASET);			// Column Address Set
	st7529_send_data(0x5);				// starting at column 5
	st7529_send_data(0x54);			// 80 Colums (3 px/column) => last column==5+80-1==0x54
	st7529_send_command(LASET);			// Line Adress Set
	st7529_send_data(0x10);			// 0x0
	st7529_send_data(0x90);			// 0x7f, 0x9F
	st7529_send_command(EXTOUT);			// EXT = 1 
	st7529_send_command(ANASET);			// Analog Circuit Set 
	st7529_send_data(0x00);
	st7529_send_data(0x01);
	st7529_send_data(0x02);				// Bias = 1/12 (0x2)
	st7529_send_command(SWINT);				// dithering off
	st7529_send_command(EXTOUT);			// EXT = 1 
	st7529_send_command(EPCTIN);				// Control EEPROM 
	st7529_send_data(0x00);
	HALL_Delay(100);
	st7529_send_command(EPMRD);				// Read from EEPROM
	HALL_Delay(100);
	st7529_send_command(EPCOUT);			
}

static void st7529_send_command(char cmd) {
	/* select command mode */
	HAL_GPIO_WritePin(GPIOD, PIN_A0, GPIO_PIN_RESET);
	/* send cmd */
	GPIOD->BSRR = CHAR_TO_BSRR(cmd);
	
	HAL_GPIO_WritePin(GPIOD, PIN_XCS, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, PIN_WR_RW, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, PIN_WR_RW, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, PIN_XCS, GPIO_PIN_SET);
}

static void st7529_send_data(char data) {
	/* select data mode */
	HAL_GPIO_WritePin(GPIOD, PIN_A0, GPIO_PIN_SET);
	/* send cmd */
	GPIOD->BSRR = CHAR_TO_BSRR(data);
	
	HAL_GPIO_WritePin(GPIOD, PIN_XCS, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, PIN_WR_RW, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, PIN_WR_RW, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, PIN_XCS, GPIO_PIN_SET);
}

static void st7529_set_column(int col0, int col1)

{
	st7529_send_command(CASET);				// Column address set
	st7529_send_data(col0 + LCD_FIRSTSEG0);
	st7529_send_data(col1 + LCD_FIRSTSEG0);
}



static void st7529_set_line(uint32_t line0, uint32_t line1)
{
	st7529_send_command(LASET);				// Line address set
	st7529_send_data(line0 + LCD_FIRSTCOM0);
	st7529_send_data(line1 + LCD_FIRSTCOM0);
}



static void LCD_Set_Addr(int Col0, int Col1, int Line0, int Line1)
{
	st7529_set_column(Col0, Col1);				// Column address set
	st7529_set_line(Line0, Line1);				// Line address set
}

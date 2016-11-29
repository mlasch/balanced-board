/*
 * file: l3gd20.h
 */
 #include <stm32f3xx.h>
 
#ifndef _L3GD20_H
#define _L3GD20_H

#define L3GD20_SPI_CLK_ENABLE()	__HAL_RCC_SPI1_CLK_ENABLE()
#define L3GD20_INT_CLK_ENABLE()	__HAL_RCC_GPIOE_CLK_ENABLE()
#define L3GD20_CS_CLK_ENABLE()	__HAL_RCC_GPIOE_CLK_ENABLE()
#define L3GD20_GPIO_CLK_ENABLE()	__HAL_RCC_GPIOA_CLK_ENABLE()

#define L3GD20_SPI	SPI1
#define L3GD20_PORT	GPIOA
#define L3GD20_INT_PORT	GPIOE
#define L3GD20_CS_PORT	GPIOE

#define L3GD20_SCK_PIN	GPIO_PIN_5
#define L3GD20_MOSI_PIN	GPIO_PIN_7
#define L3GD20_MISO_PIN	GPIO_PIN_6
#define L3GD20_CS_PIN	GPIO_PIN_3
#define L3GD20_MEMS_INT2_PIN	GPIO_PIN_1
#define L3GD20_MEMS_INT1_PIN	GPIO_PIN_0
#define L3GD20_GPIO_AF	GPIO_AF5_SPI1

#define L3GD20_READWRITE_CMD	((uint8_t)0x80)
#define L3GD20_MULTIPLEBYTE_CMD	((uint8_t)0x40)
#define SPI_DUMMY_BYTE	((uint8_t)0x00)
#define SPI_TIMEOUT	((uint32_t)0x1000)

/* L3GD20 registers */
#define WHO_AM_I	0x0f
#define CTRL_REG1	0x20
#define CTRL_REG2	0x21
#define CTRL_REG3	0x22
#define CTRL_REG4	0x23
#define CTRL_REG5	0x24
#define REFERENCE	0x25
#define OUT_TEMP	0x26
#define STATUS_REG	0x27
#define OUT_X_L	0x28
#define OUT_X_H	0x29
#define OUT_Y_L	0x2a
#define OUT_Y_H	0x2b
#define OUT_Z_L	0x2c
#define OUT_Z_H	0x2d
#define FIFO_CTRL_REG	0x2e
#define FIFO_SRC_REG	0x2f
#define INT1_CFG	0x30
#define INT1_SRC	0x31
#define INT1_TSH_XH	0x32
#define INT1_TSH_XL	0x33
#define INT1_TSH_YH	0x34
#define INT1_TSH_YL	0x35
#define INT1_TSH_ZH	0x36
#define INT1_TSH_ZL	0x37
#define INT1_DURATION	0x38

#define GYRO_CS_HIGH()	HAL_GPIO_WritePin(L3GD20_CS_PORT, L3GD20_CS_PIN, GPIO_PIN_SET);	
#define GYRO_CS_LOW()	HAL_GPIO_WritePin(L3GD20_CS_PORT, L3GD20_CS_PIN, GPIO_PIN_RESET);	

//extern uint8_t gyroBuffer[6];

void l3gd20_init(SPI_HandleTypeDef *);
void l3gd20_read(uint8_t* pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead);
void l3gd20_write(uint8_t* pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite);
void readGyro(uint8_t *);

#endif // _L3GD20_H

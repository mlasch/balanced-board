/*
 * file: lsm303dlhc.h
 */

#include <stm32f3xx.h>

#define LSM303DLHC_PORT		GPIOB
#define LSM303DLHC_INT_PORT		GPIOE
#define LSM303DLHC_GPIO_AF	GPIO_AF4_I2C1
#define LINACCEL_GPIO_CLK_ENABLE()	__HAL_RCC_GPIOB_CLK_ENABLE()
#define LINACCEL_INT_CLK_ENABLE()	__HAL_RCC_GPIOE_CLK_ENABLE()

#define LSM303DLHC_I2C I2C1
#define LSM303DLHC_SCL_PIN	GPIO_PIN_6
#define LSM303DLHC_SDA_PIN	GPIO_PIN_7
#define LSM303DLHC_I2C_TIMEOUT	0x10000
#define LINACCEL_I2C_CLK_ENABLE()	__HAL_RCC_I2C1_CLK_ENABLE()

#define LINACCEL_I2C_ADDRESS 0x32	// 0b0011001x , linear acceleration 7-bit slave address
#define GYRO_I2C_ADDRESS 0x3c

/* LSM303DLHC register addresses */
#define CTRL_REG1_A	0x20
#define CTRL_REG2_A	0x21
#define CTRL_REG3_A	0x22
#define CTRL_REG4_A 0x23
#define CTRL_REG5_A 0x24
#define CTRL_REG6_A 0x25
#define REFERENCE_A 0x26
#define STATUS_REG_A 0x27
#define OUT_X_L_A 0x28
#define OUT_X_H_A 0x29
#define OUT_Y_L_A 0x2a
#define OUT_Y_H_A 0x2b
#define OUT_Z_L_A 0x2c
#define OUT_Z_H_A 0x2d
#define FIFO_CTRL_REG_A	0x2e
#define FIFO_SRC_REG_A	0x2f
#define INT1_CFG_A	0x30
#define INT1_SRC_A	0x31
#define INT1_THS_A	0x32
#define INT1_DURATION_A	0x33
#define INT2_CFG_A	0x34
#define INT2_SRC_A	0x35
#define INT2_THS_A	0x36
#define INT2_DURATION_A	0x37
#define CLICK_CFG_A	0x38
#define CLICK_SRC_A	0x39
#define CLICK_THS_A	0x3a
#define TIME_LIMIT_A	0x3b
#define TIME_LATENCY_A	0x3c
#define TIME_WINDOW_A	0x3d
#define CRA_REG_M	0x00
#define CRB_REG_M	0x01
#define MR_REG_M	0x02
#define OUT_X_H_M	0x03
#define OUT_X_L_M	0x04
#define OUT_Z_H_M	0x05
#define OUT_Z_L_M	0x06
#define OUT_Y_H_M	0x07
#define OUT_Y_L_M	0x08
#define SR_REG_M	0x09
#define IRA_REG_M	0x0a
#define IRB_REG_M	0x0b
#define IRC_REG_M	0x0c
#define TEMP_OUT_H_M	0x31
#define TEMP_OUT_L_M 	0x32

void lsm303dlhc_init(I2C_HandleTypeDef *);
void readAccel(uint8_t *);

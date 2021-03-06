#include <stm32f3xx.h>

#include <lsm303dlhc.h>

I2C_HandleTypeDef I2C1_Handle;

/*
 * LSM303DLHC driver's private functions
 */
static uint8_t lsm303dlhc_read_reg(uint16_t addr, uint8_t reg);
static void lsm303dlhc_write_reg(uint16_t addr, uint8_t reg, uint8_t value);

uint8_t accelBuffer[6];

void readAccel(uint8_t *buffer_ptr) {
	const uint8_t startAddr = OUT_X_L_A | 0x80;		// sub-address and MSB = 1 for auto increment

	HAL_I2C_Mem_Read(&I2C1_Handle, LINACCEL_I2C_ADDRESS, startAddr, I2C_MEMADD_SIZE_8BIT, buffer_ptr, 6, LSM303DLHC_I2C_TIMEOUT);
}

void lsm303dlhc_init(I2C_HandleTypeDef *hi2c) {
	uint8_t buffer[6];
	
	/* lowlevel init */
	hi2c->Instance = I2C1;
  hi2c->Init.OwnAddress1 =  LINACCEL_I2C_ADDRESS;
  hi2c->Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c->Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c->Init.OwnAddress2 = 0;
  hi2c->Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c->Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;	
	HAL_I2C_Init(hi2c);
	
	/* write config */
	lsm303dlhc_write_reg(LINACCEL_I2C_ADDRESS, CTRL_REG1_A, 0x57);		// 100 Hz, all Axes enabled
	lsm303dlhc_write_reg(LINACCEL_I2C_ADDRESS, CTRL_REG3_A, 0x10);
	lsm303dlhc_write_reg(LINACCEL_I2C_ADDRESS, CTRL_REG4_A, 0x88);
	
	readAccel(buffer);
}

static uint8_t lsm303dlhc_read_reg(uint16_t addr, uint8_t reg) {
	uint8_t value = 0;
	
	if(HAL_I2C_Mem_Read(&I2C1_Handle, addr, reg, I2C_MEMADD_SIZE_8BIT, &value, 1, LSM303DLHC_I2C_TIMEOUT != HAL_OK)) {
		for(;;);
	}
	
	return value;
}

static void lsm303dlhc_write_reg(uint16_t addr, uint8_t reg, uint8_t value) {
	
	if (HAL_I2C_Mem_Write(&I2C1_Handle, addr, (uint16_t)reg, I2C_MEMADD_SIZE_8BIT, &value, 1, LSM303DLHC_I2C_TIMEOUT) != HAL_OK) {
		for(;;);
	}
}

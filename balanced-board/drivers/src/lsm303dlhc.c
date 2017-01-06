#include <stm32f3xx.h>

#include <lsm303dlhc.h>

I2C_HandleTypeDef I2C1_Handle;

/*
 * LSM303DLHC driver's private functions
 */
static uint8_t lsm303dlhc_read_reg(uint16_t addr, uint8_t reg);
static void lsm303dlhc_write_reg(uint16_t addr, uint8_t reg, uint8_t value);

/*
 * Read XYZ-Axis from sensor. Values are in 16-bit signed int format.
 */
void readAcclrmXYZ(uint8_t *buffer_ptr) {
	const uint8_t startAddr = OUT_X_L_A | 0x80;		// sub-address and MSB = 1 for auto increment

	HAL_I2C_Mem_Read(&I2C1_Handle, LINACCEL_I2C_ADDRESS, startAddr, I2C_MEMADD_SIZE_8BIT, buffer_ptr, 6, LSM303DLHC_I2C_TIMEOUT);
}

/*
 * 
 */
void lsm303dlhc_init(I2C_HandleTypeDef *hi2c) {
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
	lsm303dlhc_write_reg(LINACCEL_I2C_ADDRESS, CTRL_REG1_A, 0x57);		// 1 Hz, all Axes enabled 4xh=50hz 5xh=100hz 2xh=10hz
	lsm303dlhc_write_reg(LINACCEL_I2C_ADDRESS, CTRL_REG3_A, 0x10);		// I1_DRDY1=1
	
	/* High-resolution output mode=enable
	 * Block data update=output registers not updated until MSB and LSB
	 */
	lsm303dlhc_write_reg(LINACCEL_I2C_ADDRESS, CTRL_REG4_A, 0x88);
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

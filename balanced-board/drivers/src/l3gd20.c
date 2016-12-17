/*
 * L3GD20 driver's private functions
 */
#include <stm32f3xx.h>

#include <l3gd20.h>

/* globals */
SPI_HandleTypeDef SPI1_Handle;

/* private functions */
static uint8_t SPIx_WriteRead(uint8_t Byte);
static void l3gd20_write_reg(uint8_t regAddr, uint8_t value);

void readGyroXYZ(uint8_t *buffer) {
	const uint8_t startAddr = OUT_X_L;
	
	l3gd20_read(buffer, startAddr, 6);
}

void l3gd20_init(SPI_HandleTypeDef *hspi) {
	
	if(HAL_SPI_GetState(hspi) == HAL_SPI_STATE_RESET) {

		hspi->Instance = SPI1;
	
    /* Set SPI1 baudrate to 4.5 Mhz according to the demo
		 * SCL = PCLK2/SPI_BaudRatePrescaler = 72/16 = 4.5 MHz
		 * L3GD20 SCL fmax = 10 MHz
		 */
    hspi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
    hspi->Init.Direction = SPI_DIRECTION_2LINES;
    hspi->Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi->Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi->Init.CRCPolynomial = 7;
    hspi->Init.DataSize = SPI_DATASIZE_8BIT;
    hspi->Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi->Init.NSS = SPI_NSS_SOFT;
    hspi->Init.TIMode = SPI_TIMODE_DISABLE;
    hspi->Init.Mode = SPI_MODE_MASTER;
		
    //HAL_SPI_Init(hspi);
	
		GYRO_CS_HIGH();	// CS high
		
		/* configure L3GD20's registers */
		l3gd20_write_reg(CTRL_REG1, 0x0f);
		l3gd20_write_reg(CTRL_REG3, 0x08);
		l3gd20_write_reg(CTRL_REG4, 0x30);
		
		//l3gd20_read(&readBuffer, CTRL_REG1, 1);
	}
}

void l3gd20_read(uint8_t* pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead) {  
  if(NumByteToRead > 0x01) {
    ReadAddr |= (uint8_t)(L3GD20_READWRITE_CMD | L3GD20_MULTIPLEBYTE_CMD);
  } else {
    ReadAddr |= (uint8_t)L3GD20_READWRITE_CMD;
  }
	
  /* Set chip select Low at the start of the transmission */
  GYRO_CS_LOW();
  
  /* Send the Address of the indexed register */
  SPIx_WriteRead(ReadAddr);
  
  /* Receive the data that will be read from the device (MSB First) */
  while(NumByteToRead > 0x00)
  {
    /* Send dummy byte (0x00) to generate the SPI clock to GYROSCOPE (Slave device) */
    *pBuffer = SPIx_WriteRead(SPI_DUMMY_BYTE);
    NumByteToRead--;
    pBuffer++;
  }
  
  /* Set chip select High at the end of the transmission */ 
  GYRO_CS_HIGH();
}

void l3gd20_write(uint8_t* pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite)
{
  /* Configure the MS bit: 
       - When 0, the address will remain unchanged in multiple read/write commands.
       - When 1, the address will be auto incremented in multiple read/write commands.
  */
  if(NumByteToWrite > 0x01)
  {
    WriteAddr |= (uint8_t)L3GD20_MULTIPLEBYTE_CMD;
  }
  /* Set chip select Low at the start of the transmission */
  GYRO_CS_LOW();
  
  /* Send the Address of the indexed register */
  SPIx_WriteRead(WriteAddr);
  
  /* Send the data that will be written into the device (MSB First) */
  while(NumByteToWrite >= 0x01)
  {
    SPIx_WriteRead(*pBuffer);
    NumByteToWrite--;
    pBuffer++;
  }
  
  /* Set chip select High at the end of the transmission */ 
  GYRO_CS_HIGH();
}

static void l3gd20_write_reg(uint8_t regAddr, uint8_t value) {
	GYRO_CS_LOW();
	
	SPIx_WriteRead(regAddr);
	SPIx_WriteRead(value);
	
	GYRO_CS_HIGH();
}

static uint8_t SPIx_WriteRead(uint8_t Byte)
{
  uint8_t receivedbyte = 0;
  
  /* Send a Byte through the SPI peripheral */
  /* Read byte from the SPI bus */
  if(HAL_SPI_TransmitReceive(&SPI1_Handle, (uint8_t*) &Byte, (uint8_t*) &receivedbyte, 1, SPI_TIMEOUT) != HAL_OK)
  {
    // error
  }
  
  return receivedbyte;
}

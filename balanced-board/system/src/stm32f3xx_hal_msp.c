/**
  ******************************************************************************
  * @file    stm32f3xx_hal_msp_template.c
  * @author  MCD Application Team
  * @version V1.2.1
  * @date    29-April-2015
  * @brief   HAL MSP module.
  *          This file template is located in the HAL folder and should be copied 
  *          to the user folder.
  *         
  @verbatim
 ===============================================================================
                     ##### How to use this driver #####
 ===============================================================================
    [..]

  @endverbatim
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f3xx_hal.h"
#include <globals.h>

/** @addtogroup STM32F3xx_HAL_Driver
  * @{
  */

/** @defgroup HAL_MSP HAL MSP module
  * @brief HAL MSP module.
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void HAL_LED_MspInit(void);

/* Exported functions ---------------------------------------------------------*/

/** @defgroup HAL_MSP_Exported_Functions HAL MSP Exported Functions
  * @{
  */

/**
  * @brief  Initializes the Global MSP.
  * @retval None
  */
void HAL_MspInit(void)
{
	HAL_LED_MspInit();
}

/**
  * @brief  DeInitializes the Global MSP.
  * @retval None
  */
void HAL_MspDeInit(void)
{
  
}

/**
  * @brief  Initializes the I2C MSP.
  * @retval None
  */
void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{
	if (hi2c->Instance == I2C1) {
		GPIO_InitTypeDef GPIO_InitStructure;

		__HAL_RCC_I2C1_CLK_ENABLE();
		__HAL_RCC_GPIOB_CLK_ENABLE();		// I2C1 pins
		__HAL_RCC_GPIOE_CLK_ENABLE();		// INT pins
	
	/* I2C1 pins */
	GPIO_InitStructure.Pin = (GPIO_PIN_6 | GPIO_PIN_7);
  GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStructure.Pull = GPIO_PULLDOWN;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStructure.Alternate = GPIO_AF4_I2C1;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/* lsm303dlhc INT1 pin */
	GPIO_InitStructure.Pin = GPIO_PIN_4;
	GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	HAL_NVIC_SetPriority(EXTI4_IRQn, 2, 1);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);
	
	}
}

/**
  * @brief  Initializes the SPI MSP.
  * @retval None
  */
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi) {
	if (hspi->Instance == SPI1) {
		GPIO_InitTypeDef  GPIO_InitStructure;
	
		/* SPI1 pins */
		__HAL_RCC_SPI1_CLK_ENABLE();
		__HAL_RCC_GPIOE_CLK_ENABLE();
		__HAL_RCC_GPIOE_CLK_ENABLE();
		__HAL_RCC_GPIOA_CLK_ENABLE();
		
		GPIO_InitStructure.Pin = (GPIO_PIN_5 | GPIO_PIN_7 | GPIO_PIN_6);
		GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStructure.Pull  = GPIO_NOPULL; /* or GPIO_PULLDOWN */
		GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStructure.Alternate = GPIO_AF5_SPI1;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
	
		/* Chip select pins */
		GPIO_InitStructure.Pin = GPIO_PIN_3;
		GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStructure.Pull = GPIO_NOPULL;
		GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);
		
		/* Interrupt 1 & 2 pins */
		GPIO_InitStructure.Pin = (GPIO_PIN_1 | GPIO_PIN_0);
		GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;
		GPIO_InitStructure.Pull = GPIO_NOPULL;
		GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);
		
		// enable EXTI for INT2
		HAL_NVIC_SetPriority(EXTI1_IRQn, 2, 0);
		HAL_NVIC_EnableIRQ(EXTI1_IRQn);
	}
}

/**
  * @brief  Initializes the UART MSP.
  * @retval None
  */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
	if (huart->Instance == UART4) {
		/* f3 discovery:
		 * UART4_TX		PC10
		 * UART4_RX 	PC11
		 */
		GPIO_InitTypeDef GPIO_InitStructure;
		
		__HAL_RCC_GPIOC_CLK_ENABLE();
		__HAL_RCC_UART4_CLK_ENABLE();
		
		GPIO_InitStructure.Pin = (GPIO_PIN_10 | GPIO_PIN_11);
		GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStructure.Pull  = GPIO_NOPULL; /* or GPIO_PULLDOWN */
		GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStructure.Alternate = GPIO_AF5_UART4;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
	}
}

/**
  * @brief  Initializes the  MSP.
  * @retval None
  */
//void HAL_SPI_MspInit(UART_HandleTypeDef *huart)
//{
//	/* f3 discovery:
//	 * UART4_TX		PC10
//	 * UART4_RX 	PC11
//	 */

//}

/**
  * @brief  Initializes the LED MSP.
  * @retval None
  */
static void HAL_LED_MspInit(void) 
{
	/* f3 discovery
	 * LD3 (RED LED)		PE9		blinking status
	 * LD4							PE8		system error
	 */
	GPIO_InitTypeDef GPIO_InitDef;
	
	__HAL_RCC_GPIOE_CLK_ENABLE();
	
	GPIO_InitDef.Pin = (GPIO_PIN_8 |GPIO_PIN_9);
	GPIO_InitDef.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitDef.Speed = GPIO_SPEED_FREQ_LOW;
	
	HAL_GPIO_Init(GPIOE, &GPIO_InitDef);
	
	/* set the LED on startup */
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_RESET);
}

/**
  * @brief  DeInitializes the UART MSP.
  * @retval None
  */
//void HAL_UART_MspDeInit(void)
//{
//  
//}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

  /**
  ******************************************************************************
  * @file    main.h
  * @author  
  * @version V1.0.0
  * @date    
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "MQTTClient.h"  

/* WiFi x-cube related section    ---------------------------------------------------*/
#include "stm32f4xx_hal.h" 
#include "stm32f4xx_nucleo.h"
#include "wifi_interface.h"
/* WiFi x-cube related section    ---------------------------------------------------*/

/* NFC x-cube related section    ---------------------------------------------------*/
#include "cube_hal.h"
#include "lib_TagType4.h"
/* NFC x-cube related section    ---------------------------------------------------*/

/* IKS x-cuve related section    ---------------------------------------------------*/
#include "x_nucleo_iks01a1.h"
#include "x_nucleo_iks01a1_imu_6axes.h"
#include "x_nucleo_iks01a1_magneto.h"
#include "x_nucleo_iks01a1_pressure.h"
#include "x_nucleo_iks01a1_hum_temp.h"    
/* IKS x-cuve related section    ---------------------------------------------------*/

 
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define TIMx                           TIM3
#define TIMx_CLK_ENABLE()              __HAL_RCC_TIM3_CLK_ENABLE()

/* Definition for TIMx's NVIC */
#define TIMx_IRQn                      TIM3_IRQn
#define TIMx_IRQHandler                TIM3_IRQHandler


/* M24SR GPIO mapping -------------------------------------------------------------------------*/
#define M24SR_SDA_PIN 							GPIO_PIN_9
#define M24SR_SDA_PIN_PORT 						GPIOB
#define M24SR_SCL_PIN 							GPIO_PIN_8
#define M24SR_SCL_PIN_PORT 						GPIOB
#define M24SR_GPO_PIN 							GPIO_PIN_6
#define M24SR_GPO_PIN_PORT 						GPIOA
#define M24SR_RFDIS_PIN 						GPIO_PIN_7
#define M24SR_RFDIS_PIN_PORT 						GPIOA

#define INIT_CLK_GPO_RFD() 						__GPIOA_CLK_ENABLE();

/* 	I2C config	------------------------------------------------------------------------------*/
#define M24SR_I2C                  					I2C1
#define I2Cx_CLK_ENABLE()                   			 	__I2C1_CLK_ENABLE()
#define I2Cx_SDA_GPIO_CLK_ENABLE()       				__GPIOB_CLK_ENABLE()
#define I2Cx_SCL_GPIO_CLK_ENABLE()       				__GPIOB_CLK_ENABLE()

#define I2Cx_FORCE_RESET()               				__I2C1_FORCE_RESET()
#define I2Cx_RELEASE_RESET()             				__I2C1_RELEASE_RESET()


/* I2C functionality is not mapped on the same Alternate function regarding the MCU used */
		
  #define I2Cx_SCL_AF 						GPIO_AF4_I2C1
  #define M24SR_I2C_SPEED_10	                        	10000
  #define M24SR_I2C_SPEED_100					100000
  #define M24SR_I2C_SPEED_400					400000
  #define M24SR_I2C_SPEED_1000				        1000000	

 #define M24SR_I2C_SPEED				            M24SR_I2C_SPEED_400

    
#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

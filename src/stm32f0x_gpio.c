/**
  ******************************************************************************
  * @file    Template_2/stm32f0_gpio.c
  * @author  Nahuel
  * @version V1.0
  * @date    22-August-2014
  * @brief   GPIO functions.
  ******************************************************************************
  * @attention
  *
  * Use this functions to configure global inputs/outputs.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"
#include "stm32f0x_gpio.h"
#include "hard.h"



//--- Private typedef ---//
//--- Private define ---//
//--- Private macro ---//
//--- Private variables ---//
//--- Private function prototypes ---//
//--- Private functions ---//

//-------------------------------------------//
// @brief  GPIO configure.
// @param  None
// @retval None
//------------------------------------------//
void GPIO_Config (void)
{
	unsigned long temp;

	//--- MODER ---//
	//00: Input mode (reset state)
	//01: General purpose output mode
	//10: Alternate function mode
	//11: Analog mode

	//--- OTYPER ---//
	//These bits are written by software to configure the I/O output type.
	//0: Output push-pull (reset state)
	//1: Output open-drain

	//--- ORSPEEDR ---//
	//These bits are written by software to configure the I/O output speed.
	//x0: Low speed.
	//01: Medium speed.
	//11: High speed.
	//Note: Refer to the device datasheet for the frequency.

	//--- PUPDR ---//
	//These bits are written by software to configure the I/O pull-up or pull-down
	//00: No pull-up, pull-down
	//01: Pull-up
	//10: Pull-down
	//11: Reserved

#ifdef VER_1_3
	//--- GPIO A ---//
	if (!GPIOA_CLK)
		GPIOA_CLK_ON;

	temp = GPIOA->MODER;	//2 bits por pin
	temp &= 0xFF000000;		//PA0 PA1 analog input; PA2 PA3 alternative func;
							//PA4 out; PA5 analog input; PA6 alternative func; PA7 out;
#ifdef DATALOGGER
	temp |= 0x00546CAF;		//PA8 input (modifico datalogger)- PA11 out push_pull; PA12-PA15 NC
#else
	temp |= 0x00556CAF;		//PA8 - PA11 out push_pull; PA12-PA15 NC
#endif
	GPIOA->MODER = temp;

	temp = GPIOA->OTYPER;	//1 bit por pin
	temp &= 0xFFFFFFFF;		//todas push pull
	temp |= 0x00000000;
	GPIOA->OTYPER = temp;

	temp = GPIOA->OSPEEDR;	//2 bits por pin
	temp &= 0xFF003CFF;		//PA4 low PA7 low
	temp |= 0x00000000;		//PA8 - PA11 low speed
	GPIOA->OSPEEDR = temp;

	temp = GPIOA->PUPDR;	//2 bits por pin
	temp &= 0xFFFFF3FF;
	#ifdef DATALOGGER
	temp |= 0x00020000;		//PA8 pull-down
	#else
	temp |= 0x00000400;		//PA5 pull up
	#endif
	GPIOA->PUPDR = temp;

	//Alternate Fuction
	//GPIOA->AFR[0] = 0x11000000;	//PA7 -> AF1; PA6 -> AF1

	//--- GPIO B ---//
	if (!GPIOB_CLK)
		GPIOB_CLK_ON;

	temp = GPIOB->MODER;	//2 bits por pin
	temp &= 0x000C0CCC;		//PB0 out; PB2 input; PB4 out; PB6 PB7 alternative
							//PB8 input; PB10 PB11 input; PB12 - PB15 out;
#ifdef DATALOGGER
	temp |= 0x5500A103;	//PB0 analog
#else
	temp |= 0x5500A101;
#endif
	GPIOB->MODER = temp;

	temp = GPIOB->OTYPER;	//1 bit por pin
	temp &= 0xFFFFDFFE;
	temp |= 0x00002001;		//PB0 PB13 open drain
	GPIOB->OTYPER = temp;

	temp = GPIOB->OSPEEDR;	//2 bits por pin
	temp &= 0x00FFFCFC;		//PB13 high speed
	temp |= 0x0C000000;		//PB0 PB4 PB12-PB15 low speed
	GPIOB->OSPEEDR = temp;

	temp = GPIOB->PUPDR;	//2 bits por pin
	temp &= 0xFF0CFFCF;		//PB2 pull up; PB8 pull dwn
	temp |= 0x00520010;		//PB10 PB11 pull up
	GPIOB->PUPDR = temp;

	//Alternate Fuction
	//GPIOB->AFR[0] = 0x11000000;	//PA7 -> AF1; PA6 -> AF1

	//--- GPIO C ---//
	if (!GPIOC_CLK)
		GPIOC_CLK_ON;

	temp = GPIOC->MODER;	//2 bits por pin
	temp &= 0xF3FFFFFF;		//PC13 input
	temp |= 0x00000000;
	GPIOC->MODER = temp;

	temp = GPIOC->OTYPER;	//1 bit por pin
	temp &= 0xFFFFFFFF;
	temp |= 0x00000000;
	GPIOC->OTYPER = temp;

	temp = GPIOC->OSPEEDR;	//2 bits por pin
	temp &= 0xFFFFFFFF;
	temp |= 0x00000000;
	GPIOC->OSPEEDR = temp;

	temp = GPIOC->PUPDR;	//2 bits por pin
	temp &= 0xF3FFFFFF;		//PC13 pull up
	temp |= 0x04000000;
	GPIOC->PUPDR = temp;

	//Alternate Fuction
	//GPIOC->AFR[0] = 0x11000000;	//PA7 -> AF1; PA6 -> AF1

#endif

#ifdef VER_1_2
	//--- GPIO A ---//
	if (!GPIOA_CLK)
		GPIOA_CLK_ON;

	temp = GPIOA->MODER;	//2 bits por pin
	temp &= 0x0C000000;		//PA0 - PA5 out push_pull; PA6 PA7 alternate function;
	temp |= 0x0168A555;		//PA8 input; PA9 PA10 alternativa; PA11 PA12 out push_pull; PA14 PA15 input
	GPIOA->MODER = temp;

	temp = GPIOA->OTYPER;	//1 bit por pin
	temp &= 0xFFFFE700;
	temp |= 0x00000000;		//PA0 a PA7 push pull; PA11 PA12 push pull
	GPIOA->OTYPER = temp;

	temp = GPIOA->OSPEEDR;	//2 bits por pin
	temp &= 0xFC3F0000;
	temp |= 0x00000000;		//low speed
	GPIOA->OSPEEDR = temp;

	temp = GPIOA->PUPDR;	//2 bits por pin
	temp &= 0x0FFFFFFF;
	temp |= 0x50000000;		//PA14 PA15 con pullup
	GPIOA->PUPDR = temp;

	//Alternate Fuction
	//GPIOA->AFR[0] = 0x11000000;	//PA7 -> AF1; PA6 -> AF1

	//--- GPIO B ---//
	if (!GPIOB_CLK)
		GPIOB_CLK_ON;

	temp = GPIOB->MODER;	//2 bits por pin
	temp &= 0xFFFF0030;		//PB0 PB1 analog input; PB3 input; PB4 output; PB5 input; PB6 PB7 output
	temp |= 0x0000510F;
	GPIOB->MODER = temp;

	temp = GPIOB->OTYPER;	//1 bit por pin
	temp &= 0xFFFFFF3F;
	temp |= 0x00000040;		//PB6 open drain PB7 push pull
	GPIOB->OTYPER = temp;

	temp = GPIOB->OSPEEDR;	//2 bits por pin
	temp &= 0xFFFF0FFF;
	temp |= 0x00000000;		//low speed
	GPIOB->OSPEEDR = temp;

	temp = GPIOB->PUPDR;	//2 bits por pin
	temp &= 0xFFFFF33F;		//PB3 PB5 pull up
	temp |= 0x00000440;
	GPIOB->PUPDR = temp;

	//Alternate Fuction
	//GPIOB->AFR[0] = 0x11000000;	//PA7 -> AF1; PA6 -> AF1
#endif

#ifdef VER_1_0
	//--- GPIO A ---//
	if (!GPIOA_CLK)
		GPIOA_CLK_ON;

	temp = GPIOA->MODER;	//2 bits por pin
	temp &= 0x3C000000;		//PA0 - PA5 out push_pull; PA6 alternate function; PA7 out push pull;
	temp |= 0x01686555;		//PA8 input; PA9 PA10 alternativa; PA11 PA12 out push_pull; PA15 input
	GPIOA->MODER = temp;

	temp = GPIOA->OTYPER;	//1 bit por pin
	temp &= 0xFFFFE700;
	temp |= 0x00000000;		//PA0 a PA7 push pull; PA11 PA12 push pull
	GPIOA->OTYPER = temp;

	temp = GPIOA->OSPEEDR;	//2 bits por pin
	temp &= 0xFC3F0000;
	temp |= 0x00000000;		//low speed
	GPIOA->OSPEEDR = temp;

	temp = GPIOA->PUPDR;	//2 bits por pin
	temp &= 0x3FFFFFFF;
	temp |= 0x40000000;		//PA15 con pullup
	GPIOA->PUPDR = temp;



	//Alternate Fuction
	//GPIOA->AFR[0] = 0x11000000;	//PA7 -> AF1; PA6 -> AF1

	//--- GPIO B ---//
	if (!GPIOB_CLK)
		GPIOB_CLK_ON;

	temp = GPIOB->MODER;	//2 bits por pin
	temp &= 0xFFFF0F30;		//PB0 analog input; PB1 PB3 input PB6 PB7 output
	temp |= 0x00005003;
	GPIOB->MODER = temp;

	temp = GPIOB->OTYPER;	//1 bit por pin
	temp &= 0xFFFFFF3F;
	temp |= 0x00000040;		//PB6 open drain PB7 push pull
	GPIOB->OTYPER = temp;

	temp = GPIOB->OSPEEDR;	//2 bits por pin
	temp &= 0xFFFF0FFF;
	temp |= 0x00000000;		//low speed
	GPIOB->OSPEEDR = temp;

	temp = GPIOB->PUPDR;	//2 bits por pin
	temp &= 0xFFFFFF33;		//PB1 PB3 pull up
	temp |= 0x00000044;
	GPIOB->PUPDR = temp;

	//Alternate Fuction
	//GPIOB->AFR[0] = 0x11000000;	//PA7 -> AF1; PA6 -> AF1
#endif

#ifdef GPIOF_ENABLE

	//--- GPIO F ---//
	if (!GPIOF_CLK)
		GPIOF_CLK_ON;

	temp = GPIOF->MODER;
	temp &= 0xFFFFFFFF;
	temp |= 0x00000000;
	GPIOF->MODER = temp;

	temp = GPIOF->OTYPER;
	temp &= 0xFFFFFFFF;
	temp |= 0x00000000;
	GPIOF->OTYPER = temp;

	temp = GPIOF->OSPEEDR;
	temp &= 0xFFFFFFFF;
	temp |= 0x00000000;
	GPIOF->OSPEEDR = temp;

	temp = GPIOF->PUPDR;
	temp &= 0xFFFFFFFF;
	temp |= 0x00000000;
	GPIOF->PUPDR = temp;

#endif


	//Interrupt en PB8
	if (!SYSCFG_CLK)
		SYSCFG_CLK_ON;

	SYSCFG->EXTICR[2] = 0x00000001; //Select Port B & Pin 8 external interrupt
	EXTI->IMR |= 0x0100; 			//Corresponding mask bit for interrupts
	EXTI->EMR |= 0x0000; 			//Corresponding mask bit for events
	EXTI->RTSR |= 0x0100; 			//Pin 8 Interrupt line on rising edge
	EXTI->FTSR |= 0x0100; 			//Pin 8 Interrupt line on falling edge

	NVIC_EnableIRQ(EXTI4_15_IRQn);
	NVIC_SetPriority(EXTI4_15_IRQn, 6);


}

inline void EXTIOff (void)
{
	EXTI->IMR &= ~0x00000100;
}

inline void EXTIOn (void)
{
	EXTI->IMR |= 0x00000100;
}

//--- end of file ---//

/*
 * hard.h
 *
 *  Created on: 28/11/2013
 *      Author: Mariano
 */

#ifndef HARD_H_
#define HARD_H_


//----------- Defines For Configuration -------------
//----------- Hardware Board Version -------------
//#define VER_1_0
//#define VER_1_2
#define VER_1_3

//-------- WiFi Configuration -------------
//#define USE_HLK_WIFI
//#define USE_ESP_WIFI

//-------- Type of Program ----------------
//#define WIFI_TO_CEL_PHONE_PROGRAM
//#define WIFI_TO_MQTT_BROKER
//#define USE_CERT_PROGRAM
//#define USE_PROD_PROGRAM
// #define USE_PROD_PROGRAM_ONLY_LDR
//#define MQTT_MEM_ONLY
#define DATALOGGER

//-------- Hardware resources for Type of Program ----------------
#ifdef DATALOGGER
#define DATALOGGER_FILTER	32
#define DATALOGGER_FILTER_DIVISOR	5
#define UPDATE_INFO	10000		//tiempo en ms en que manda paquetes
#define UPDATE_FILTER	(UPDATE_INFO >> DATALOGGER_FILTER_DIVISOR)
#endif

#ifdef WIFI_TO_CEL_PHONE_PROGRAM
#define USE_DMX
#endif

#ifdef WIFI_TO_MQTT_BROKER
#define USE_SUBSCRIBE
#endif

#ifdef USE_CERT_PROGRAM
#define USE_DMX
#endif

#ifdef USE_PROD_PROGRAM
#define USE_DMX
#endif

#ifdef MQTT_MEM_ONLY
//#define SUBSCRIBE
#endif


//-------- End Of Defines For Configuration ------

#ifdef VER_1_0
//GPIOA pin0
//GPIOA pin1
//GPIOA pin2
//GPIOA pin3	interface a LCD

//GPIOA pin4
#define LCD_E ((GPIOA->ODR & 0x0010) != 0)
#define LCD_E_ON	GPIOA->BSRR = 0x00000010
#define LCD_E_OFF GPIOA->BSRR = 0x00100000

//GPIOA pin5
#define LCD_RS ((GPIOA->ODR & 0x0020) != 0)
#define LCD_RS_ON	GPIOA->BSRR = 0x00000020
#define LCD_RS_OFF GPIOA->BSRR = 0x00200000

//GPIOA pin6
//para PWM_CH1

//GPIOA pin7
#define CTRL_BKL ((GPIOA->ODR & 0x0080) != 0)
#define CTRL_BKL_ON	GPIOA->BSRR = 0x00000080
#define CTRL_BKL_OFF GPIOA->BSRR = 0x00800000

//GPIOA pin8
#define EXTI_Input ((GPIOA->IDR & 0x0100) != 0)
#define DMX_INPUT EXTI_Input

//GPIOA pin9
//GPIOA pin10	usart tx rx

//GPIOA pin11
#define RELAY ((GPIOA->ODR & 0x0800) != 0)
#define RELAY_ON	GPIOA->BSRR = 0x00000800
#define RELAY_OFF GPIOA->BSRR = 0x08000000

//GPIOA pin12
#define SW ((GPIOA->ODR & 0x1000) != 0)
#define SW_TX	GPIOA->BSRR = 0x00001000
#define SW_RX GPIOA->BSRR = 0x10000000

//GPIOA pin13
//GPIOA pin14

//GPIOA pin15
#define SW_AC ((GPIOA->IDR & 0x8000) == 0)	//activo por 0

//GPIOB pin0

//GPIOB pin1
#define S2_PIN ((GPIOB->IDR & 0x0002) == 0)

//GPIOB pin3
#define S1_PIN ((GPIOB->IDR & 0x0008) == 0)

//GPIOB pin4
//GPIOB pin5

//GPIOB pin6
#define DMX_TX_PIN ((GPIOB->ODR & 0x0040) == 0)
#define DMX_TX_PIN_OFF	GPIOB->BSRR = 0x00000040
#define DMX_TX_PIN_ON GPIOB->BSRR = 0x00400000

//GPIOB pin7
#define LED ((GPIOB->ODR & 0x0080) != 0)
#define LED_ON	GPIOB->BSRR = 0x00000080
#define LED_OFF GPIOB->BSRR = 0x00800000

#define CH_IN_TEMP ADC_Channel_0

#endif	//

#ifdef VER_1_2
//GPIOA pin0
//GPIOA pin1
//GPIOA pin2
//GPIOA pin3	interface a LCD

//GPIOA pin4
#define LCD_E ((GPIOA->ODR & 0x0010) != 0)
#define LCD_E_ON	GPIOA->BSRR = 0x00000010
#define LCD_E_OFF GPIOA->BSRR = 0x00100000

//GPIOA pin5
#define LCD_RS ((GPIOA->ODR & 0x0020) != 0)
#define LCD_RS_ON	GPIOA->BSRR = 0x00000020
#define LCD_RS_OFF GPIOA->BSRR = 0x00200000

//GPIOA pin6
//para PWM_CH1

//GPIOA pin7
//para PWM_CH2


//GPIOB pin0
//ADC_Current

//GPIOB pin1
//ADC_LDR

//GPIOA pin8
#define EXTI_Input ((GPIOA->IDR & 0x0100) != 0)
#define DMX_INPUT EXTI_Input

//GPIOA pin9
//GPIOA pin10	usart tx rx

//GPIOA pin11
#define RELAY ((GPIOA->ODR & 0x0800) != 0)
#define RELAY_ON	GPIOA->BSRR = 0x00000800
#define RELAY_OFF GPIOA->BSRR = 0x08000000

//GPIOA pin12
#define SW ((GPIOA->ODR & 0x1000) != 0)
#define SW_TX	GPIOA->BSRR = 0x00001000
#define SW_RX GPIOA->BSRR = 0x10000000

//GPIOA pin13
//GPIOA pin14
#define S2_PIN ((GPIOA->IDR & 0x4000) == 0)

//GPIOA pin15
#define SW_AC ((GPIOA->IDR & 0x8000) == 0)	//activo por 0

//GPIOB pin3
#define S1_PIN ((GPIOB->IDR & 0x0008) == 0)

//GPIOB pin4
#define CTRL_BKL ((GPIOB->ODR & 0x0010) != 0)
#define CTRL_BKL_ON	GPIOB->BSRR = 0x00000010
#define CTRL_BKL_OFF GPIOB->BSRR = 0x00100000


//GPIOB pin5
#define MOV_SENSE ((GPIOB->IDR & 0x0020) == 0)	//activo por 0

//GPIOB pin6
#define DMX_TX_PIN ((GPIOB->ODR & 0x0040) == 0)
#define DMX_TX_PIN_OFF	GPIOB->BSRR = 0x00000040
#define DMX_TX_PIN_ON GPIOB->BSRR = 0x00400000

//GPIOB pin7
#define LED ((GPIOB->ODR & 0x0080) != 0)
#define LED_ON	GPIOB->BSRR = 0x00000080
#define LED_OFF GPIOB->BSRR = 0x00800000

#define CH_IN_TEMP ADC_Channel_0

#endif	//

#ifdef VER_1_3
//GPIOC pin13
#define SW_AC ((GPIOC->IDR & 0x2000) == 0)	//activo por 0

//GPIOA pin0	analog input	I_Sense
//GPIOA pin1	analog input	V_Sense

//GPIOA pin2
//GPIOA pin3	USART2

//GPIOA pin4
#define RELAY ((GPIOA->ODR & 0x0010) != 0)
#define RELAY_ON	GPIOA->BSRR = 0x00000010
#define RELAY_OFF GPIOA->BSRR = 0x00100000

//GPIOA pin5	analog input LDR

//GPIOA pin6	TIM3_CH1 para PWM_CH1

//GPIOA pin7
#define LED ((GPIOA->ODR & 0x0080) != 0)
#define LED_ON	GPIOA->BSRR = 0x00000080
#define LED_OFF GPIOA->BSRR = 0x00800000

//GPIOB pin0
#define WRST ((GPIOB->ODR & 0x0001) != 0)
#define WRST_ON	GPIOB->BSRR = 0x00000001
#define WRST_OFF GPIOB->BSRR = 0x00010000

//GPIOB pin1

//GPIOB pin2	input
#define MOV_SENSE ((GPIOB->IDR & 0x0004) == 0)

//GPIOB pin10	input
#define S2_PIN ((GPIOB->IDR & 0x0400) == 0)

//GPIOB pin11	input
#define S1_PIN ((GPIOB->IDR & 0x0800) == 0)

//GPIOB pin12
#define CTRL_BKL ((GPIOB->ODR & 0x1000) != 0)
#define CTRL_BKL_ON	GPIOB->BSRR = 0x00001000
#define CTRL_BKL_OFF GPIOB->BSRR = 0x10000000

//GPIOB pin13
#define DMX_TX_PIN ((GPIOB->ODR & 0x2000) == 0)
#define DMX_TX_PIN_OFF	GPIOB->BSRR = 0x00002000
#define DMX_TX_PIN_ON GPIOB->BSRR = 0x20000000

//GPIOB pin14
#define LCD_RS ((GPIOB->ODR & 0x4000) != 0)
#define LCD_RS_ON	GPIOB->BSRR = 0x00004000
#define LCD_RS_OFF GPIOB->BSRR = 0x40000000

//GPIOB pin15
#define LCD_E ((GPIOB->ODR & 0x8000) != 0)
#define LCD_E_ON	GPIOB->BSRR = 0x00008000
#define LCD_E_OFF GPIOB->BSRR = 0x80000000

//GPIOA pin8
#ifdef DATALOGGER
#define LOGGER_INPUT ((GPIOA->IDR & 0x0100) != 0)
#endif
//GPIOA pin9
//GPIOA pin10
//GPIOA pin11	interface LCD

//GPIOA pin12
//GPIOA pin13

//GPIOF pin6
//GPIOF pin7

//GPIOA pin14
//GPIOA pin15

//GPIOB pin3

//GPIOB pin4
#define SW ((GPIOB->ODR & 0x0010) != 0)
#define SW_TX	GPIOB->BSRR = 0x00000010
#define SW_RX GPIOB->BSRR = 0x00100000

//GPIOB pin5

//GPIOB pin6
//GPIOB pin7	USART1

//GPIOB pin8
#define EXTI_Input ((GPIOB->IDR & 0x0100) != 0)
#define DMX_INPUT EXTI_Input



//GPIOB pin7

#define CH_IN_TEMP ADC_Channel_0

#endif	//

//#define DOOR_ROOF	200
//#define DOOR_THRESH	180


//ESTADOS DEL PROGRAMA PRINCIPAL
#define MAIN_INIT				0
#define MAIN_INIT_1				1
#define MAIN_SENDING_CONF		2
#define MAIN_WAIT_CONNECT_0		3
#define MAIN_WAIT_CONNECT_1		4
#define MAIN_WAIT_CONNECT_2		5
#define MAIN_READING_TCP		6
#define MAIN_TRANSPARENT		7
#define MAIN_AT_CONFIG_2B		8
#define MAIN_ERROR				9

#define MAIN_STAND_ALONE		10
#define MAIN_GROUPED			11
#define MAIN_NETWORKED			12
#define MAIN_NETWORKED_1		13
#define MAIN_IN_MAIN_MENU		14

#define MAIN_SWITCH_ONLY		20



//---- Temperaturas en el LM335
//37	2,572
//40	2,600
//45	2,650
//50	2,681
//55	2,725
//60	2,765
#define TEMP_IN_30		3226
#define TEMP_IN_35		3279
#define TEMP_IN_50		3434
#define TEMP_IN_65		3591
#define TEMP_DISCONECT		4000

//ESTADOS DEL DISPLAY EN RGB_FOR_CAT
#define SHOW_CHANNELS	0
#define SHOW_NUMBERS	1

#define SWITCHES_TIMER_RELOAD	10
#define AC_SWITCH_TIMER_RELOAD	22

#define SWITCHES_THRESHOLD_FULL	300		//3 segundos
#define SWITCHES_THRESHOLD_HALF	100		//1 segundo
#define SWITCHES_THRESHOLD_MIN	5		//50 ms

#define AC_SWITCH_THRESHOLD_ROOF	255		//techo del integrador
#define AC_SWITCH_THRESHOLD_FULL	136		//3 segundos
#define AC_SWITCH_THRESHOLD_HALF	45		//1 segundo
#define AC_SWITCH_THRESHOLD_MIN		2		//50 ms

#define TTIMER_FOR_CAT_DISPLAY			2000	//tiempo entre que dice canal y el numero
#define TIMER_STANDBY_TIMEOUT_REDUCED	2000	//reduced 2 segs
#define TIMER_STANDBY_TIMEOUT			6000	//6 segundos
#define DMX_DISPLAY_SHOW_TIMEOUT		30000	//30 segundos

#define S_FULL		10
#define S_HALF		3
#define S_MIN		1
#define S_NO		0

#define FUNCTION_DMX	1
#define FUNCTION_MAN	2
#define FUNCTION_CAT	FUNCTION_MAN

#define SIZEOF_DATA1	512
#define SIZEOF_DATA		256
#define SIZEOF_DATA512	SIZEOF_DATA1
#define SIZEOF_DATA256	SIZEOF_DATA
#define SIZEOF_BUFFTCP	SIZEOF_DATA





// ------- de los switches -------
void UpdateSwitches (void);
unsigned char CheckS1 (void);
unsigned char CheckS2 (void);
void UpdateACSwitch (void);
unsigned char CheckACSw (void);


#endif /* HARD_H_ */

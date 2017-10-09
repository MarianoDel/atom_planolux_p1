/*
 * adc.c
 *
 *  Created on: 04/05/2015
 *      Author: Mariano
 */
#include "adc.h"
#include "stm32f0xx.h"
#include "hard.h"

#include "stm32f0xx_adc.h"
//la incluyo por constates como ADC_SampleTime_239_5Cycles


//--- VARIABLES EXTERNAS ---//
extern volatile unsigned short adc_ch [];
extern volatile unsigned short take_temp_sample;
extern volatile unsigned short take_ldr_sample;
extern unsigned char new_ldr_sample;			//la uso para avisar que se puede usar el filtro

#ifdef ADC_WITH_INT
extern volatile unsigned char seq_ready;
#endif

//--- VARIABLES GLOBALES ---//
#define SIZEOF_BOARD_TEMP	8
unsigned short board_temp [SIZEOF_BOARD_TEMP];
unsigned short last_temp = 0;
unsigned char board_temp_index = 0;
unsigned char new_temp_sample = 0;			//la uso para avisar que se puede usar el filtro

#define SIZEOF_LDR	16
unsigned short vldr [SIZEOF_LDR];
unsigned char ldr_index = 0;


void AdcConfig (void)
{
#ifdef ADC_WITH_INT
	NVIC_InitTypeDef    NVIC_InitStructure;
#endif
	unsigned short cal = 0;

	if (!RCC_ADC_CLK)
		RCC_ADC_CLK_ON;

	// preseteo los registros a default, la mayoria necesita tener ADC apagado
	ADC1->CR = 0x00000000;
	ADC1->IER = 0x00000000;
	ADC1->CFGR1 = 0x00000000;
	ADC1->CFGR2 = 0x00000000;
	ADC1->SMPR = 0x00000000;
	ADC1->TR = 0x0FFF0000;
	ADC1->CHSELR = 0x00000000;

	//set clock
	ADC1->CFGR2 = ADC_ClockMode_SynClkDiv4;

	//set resolution & trigger
	//ADC1->CFGR1 |= ADC_Resolution_10b | ADC_ExternalTrigConvEdge_Rising | ADC_ExternalTrigConv_T3_TRGO;
	//ADC1->CFGR1 |= ADC_Resolution_12b | ADC_ExternalTrigConvEdge_Rising | ADC_ExternalTrigConv_T1_TRGO;
	//ADC1->CFGR1 |= ADC_DMAMode_Circular | 0x00000001;
	ADC1->CFGR1 |= ADC_Resolution_12b;

	//ADC1->CFGR1 |= ADC_Resolution_10b | ADC_ExternalTrigConvEdge_Falling | ADC_ExternalTrigConv_T3_TRGO;

	//set sampling time
	//ADC1->SMPR |= ADC_SampleTime_41_5Cycles;		//17.39 son SP 420
	//ADC1->SMPR |= ADC_SampleTime_28_5Cycles;		//17.39 son SP 420
	//ADC1->SMPR |= ADC_SampleTime_7_5Cycles;		//17.36 de salida son SP 420 pero a veces pega
													//las dos int (usar DMA?) y pierde el valor intermedio
	//ADC1->SMPR |= ADC_SampleTime_1_5Cycles;			//20.7 de salida son SP 420 (regula mal)
	ADC1->SMPR |= ADC_SampleTime_239_5Cycles;

#ifdef VER_1_2
	//set channel selection
	ADC1->CHSELR |= ADC_Channel_0 | ADC_Channel_1 | ADC_Channel_2 | ADC_Channel_3 | ADC_Channel_4;
	//ADC1->CHSELR |= ADC_Channel_0 | ADC_Channel_1 | ADC_Channel_2;
	//ADC1->CHSELR |= ADC_Channel_0 | ADC_Channel_1;
	//ADC1->CHSELR |= ADC_Channel_2;	//individuales andan todos
#endif
#ifdef VER_1_3
	//set channel selection
	ADC1->CHSELR |= ADC_Channel_0 | ADC_Channel_1 | ADC_Channel_5;
#endif

	//habilito sensado de temperatura
	ADC->CCR |= ADC_CCR_TSEN;

#ifdef ADC_WITH_INT
	//set interrupts
	ADC1->IER |= ADC_IT_EOC;

	/* Configure and enable ADC1 interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = ADC1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif

	//calibrar ADC
	cal = ADC_GetCalibrationFactor(ADC1);

	// Enable ADC1
	ADC1->CR |= ADC_CR_ADEN;
}

#ifdef ADC_WITH_INT
void ADC1_COMP_IRQHandler (void)
{
	/*
	if (ADC1->ISR & ADC_IT_EOC)
	{
		LED_ON;
		//clear pending
		ADC1->ISR |= ADC_IT_EOC | ADC_IT_EOSEQ;
		LED_OFF;
	}
	*/


	if (ADC1->ISR & ADC_IT_EOC)
	{
		//LED_ON;
		if (ADC1->ISR & ADC_IT_EOSEQ)	//seguro que es channel2
		{
			adc_ch2 = ADC1->DR;
			seq_ready = 1;
		}
		else
		{
//			LED_ON;
			adc_ch1 = ADC1->DR;
		}

		//clear pending
		ADC1->ISR |= ADC_IT_EOC | ADC_IT_EOSEQ;

		//LED_OFF;
	}
}
#endif


unsigned short ADC_Conf (void)
{
	unsigned short cal = 0;
	ADC_InitTypeDef ADC_InitStructure;

	if (!RCC_ADC_CLK)
		RCC_ADC_CLK_ON;

	ADC_ClockModeConfig(ADC1, ADC_ClockMode_SynClkDiv4);

	// preseteo de registros a default
	  /* ADCs DeInit */
	  ADC_DeInit(ADC1);

	  /* Initialize ADC structure */
	  ADC_StructInit(&ADC_InitStructure);

	  /* Configure the ADC1 in continuous mode with a resolution equal to 12 bits  */
	  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	  ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;
	  ADC_Init(ADC1, &ADC_InitStructure);

	//software by setting bit ADCAL=1.
	//Calibration can only be initiated when the ADC is disabled (when ADEN=0).
	//ADCAL bit stays at 1 during all the calibration sequence.
	//It is then cleared by hardware as soon the calibration completes
	cal = ADC_GetCalibrationFactor(ADC1);

	// Enable ADC1
	ADC_Cmd(ADC1, ENABLE);

	SetADC1_SampleTime ();

	return cal;
}

unsigned short ReadADC1 (unsigned int channel)
{
	uint32_t tmpreg = 0;
	//GPIOA_PIN4_ON;
	// Set channel and sample time
	//ADC_ChannelConfig(ADC1, channel, ADC_SampleTime_7_5Cycles);	//pifia la medicion 2800 o 3400 en ves de 4095
	//ADC_ChannelConfig(ADC1, channel, ADC_SampleTime_239_5Cycles);
	//ADC_ChannelConfig(ADC1, ADC_Channel_0, ADC_SampleTime_239_5Cycles);

	//ADC_ChannelConfig INTERNALS
	/* Configure the ADC Channel */
	ADC1->CHSELR = channel;

	/* Clear the Sampling time Selection bits */
	tmpreg &= ~ADC_SMPR1_SMPR;

	/* Set the ADC Sampling Time register */
	tmpreg |= (uint32_t)ADC_SampleTime_239_5Cycles;

	/* Configure the ADC Sample time register */
	ADC1->SMPR = tmpreg ;


	// Start the conversion
	ADC_StartOfConversion(ADC1);
	// Wait until conversion completion
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
	// Get the conversion value
	//GPIOA_PIN4_OFF;	//tarda 20us en convertir
	return ADC_GetConversionValue(ADC1);
}

//Setea el sample time en el ADC
void SetADC1_SampleTime (void)
{
	uint32_t tmpreg = 0;

	/* Clear the Sampling time Selection bits */
	tmpreg &= ~ADC_SMPR1_SMPR;

	/* Set the ADC Sampling Time register */
	tmpreg |= (uint32_t)ADC_SampleTime_239_5Cycles;

	/* Configure the ADC Sample time register */
	ADC1->SMPR = tmpreg ;
}


//lee el ADC sin cambiar el sample time anterior
unsigned short ReadADC1_SameSampleTime (unsigned int channel)
{
	// Configure the ADC Channel
	ADC1->CHSELR = channel;

	// Start the conversion
	ADC1->CR |= (uint32_t)ADC_CR_ADSTART;

	// Wait until conversion completion
	while((ADC1->ISR & ADC_ISR_EOC) == 0);

	// Get the conversion value
	return (uint16_t) ADC1->DR;
}

unsigned short ReadADC1Check (unsigned char channel)
{
	if (ADC1->CR & 0x01)			//reviso ADEN
		return 0xFFFF;

	//espero que este listo para convertir
	while ((ADC1->ISR & 0x01) == 0);	//espero ARDY = 1

	if ((ADC1->CFGR1 & 0x00010000) == 0)			//reviso DISCONTINUOS = 1
		return 0xFFFF;

	if (ADC1->CFGR1 & 0x00002000)					//reviso CONT = 0
		return 0xFFFF;

	if (ADC1->CFGR1 & 0x00000C00)					//reviso TRIGGER = 00
		return 0xFFFF;

	if (ADC1->CFGR1 & 0x00000020)					//reviso ALIGN = 0
		return 0xFFFF;

	if (ADC1->CFGR1 & 0x00000018)					//reviso RES = 00
		return 0xFFFF;

	//espero que no se este convirtiendo ADCSTART = 0
	while ((ADC1->CR & 0x02) != 0);	//espero ADCSTART = 0

	ADC1->CHSELR = 0x00000001;	//solo convierto CH0

	return 1;
}

unsigned short GetLDR (void)
{
	unsigned char i;
	unsigned short total_sum = 0;

	for (i = 0; i < SIZEOF_LDR; i++)
		total_sum += vldr [i];

	return (total_sum >> 4);
}

void UpdateLDR(void)
{
	//hago update cada 1 seg
	if (!take_ldr_sample)
	{
		take_ldr_sample = 1000;

		vldr [ldr_index] = ReadADC1_SameSampleTime(ADC_Channel_5);
		if (ldr_index < SIZEOF_LDR)
			ldr_index++;
		else
			ldr_index = 0;

		new_ldr_sample = 1;
	}
}


void UpdateTemp(void)
{
	//hago update cada 1 seg
	if (!take_temp_sample)
	{
		take_temp_sample = 1000;

		if (board_temp_index < SIZEOF_BOARD_TEMP)
			board_temp_index++;
		else
			board_temp_index = 0;

		board_temp [board_temp_index] = ReadADC1_SameSampleTime(ADC_Channel_16);
		new_temp_sample = 1;
	}
}

unsigned short GetTemp (void)
{
	unsigned char i;
	unsigned int t = 0;

	if (new_temp_sample)
	{
		for (i = 0; i < SIZEOF_BOARD_TEMP; i++)
		{
			t += board_temp[i];
		}

		last_temp = t >> 3;
		new_temp_sample = 0;
	}

	return last_temp;
}

short ConvertTemp (unsigned short t_sample)
{
	short dy = 110 - 30;
	short dt = 0;
	short temp = 0;

	dt = *TEMP110_CAL_ADDR - *TEMP30_CAL_ADDR;

	temp = t_sample - *TEMP30_CAL_ADDR;
	temp = temp * dy;
	temp = temp / dt;
	//temp = temp + 30;
	temp = temp + 20;	//resto 10 para compensar por temperatura exterior

	return temp;
}

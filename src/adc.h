/*
 * adc.h
 *
 *  Created on: 10/09/2014
 *      Author: Mariano
 */

#ifndef ADC_H_
#define ADC_H_

#define RCC_ADC_CLK 		(RCC->APB2ENR & 0x00000200)
#define RCC_ADC_CLK_ON 		RCC->APB2ENR |= 0x00000200
#define RCC_ADC_CLK_OFF 	RCC->APB2ENR &= ~0x00000200


/* Temperature sensor calibration value address */
#define TEMP110_CAL_ADDR ((uint16_t*) ((uint32_t) 0x1FFFF7C2))
#define TEMP30_CAL_ADDR ((uint16_t*) ((uint32_t) 0x1FFFF7B8))
//#define VDD_CALIB ((uint16_t) (330))
//#define VDD_APPLI ((uint16_t) (300))


unsigned short ADC_Conf (void);
unsigned short ReadADC1 (unsigned int);
unsigned short ReadADC1_SameSampleTime (unsigned int);
void SetADC1_SampleTime (void);
unsigned short ReadADC1Check (unsigned char);
void UpdateTemp(void);
unsigned short GetTemp (void);
void AdcConfig (void);
short ConvertTemp (unsigned short);
unsigned short GetLDR (void);
void UpdateLDR(void);

#endif /* ADC_H_ */

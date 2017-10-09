/**
  ******************************************************************************
  * @file    Template_2/main.h
  * @author  Nahuel
  * @version V1.0
  * @date    22-August-2014
  * @brief   Header for gpio module
  ******************************************************************************
  * @attention
  *
  *
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F0X_UART_H
#define __STM32F0X_UART_H

//--- Exported types ---//
//--- Exported constants ---//
//--- Exported macro ---//
#define USART1_CLK (RCC->APB2ENR & 0x00004000)
#define USART1_CLK_ON RCC->APB2ENR |= 0x00004000
#define USART1_CLK_OFF RCC->APB2ENR &= ~0x00004000

#define USART2_CLK (RCC->APB1ENR & 0x00020000)
#define USART2_CLK_ON RCC->APB1ENR |= 0x00020000
#define USART2_CLK_OFF RCC->APB1ENR &= ~0x00020000

#define USART_9600		5000
#define USART_115200		416
#define USART_250000		192

#define USARTx_RX_DISA	USARTx->CR1 &= 0xfffffffb
#define USARTx_RX_ENA	USARTx->CR1 |= 0x04

//modos de recepcion de paquetes
#define PCKT_NOT_READY		0
#define PCKT_DMX			1
#define PCKT_RDM			2

//--- Exported functions ---//
void USART1Config(void);
void USART1_IRQHandler(void);
void USARTSendSingle(unsigned char);

void UsartSendDMX (void);

void USARTSend(char *);
void USARTSendUnsigned(unsigned char *, unsigned char);

void USART2Config(void);
void USART2_IRQHandler(void);
void Usart2Send (char *);
void Usart2SendUnsigned(unsigned char *, unsigned char);

#endif //--- End ---//


//--- END OF FILE ---//

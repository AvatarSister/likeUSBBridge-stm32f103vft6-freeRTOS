/**
  ******************************************************************************
  * @file    
  * @author  
  * @version 
  * @date    
  * @brief   
  ******************************************************************************
  * @attention
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SPI_H
#define __SPI_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#define SPI2_CLOCK          RCC_APB1Periph_SPI2
#define SPI2_GPIO_GROUP     GPIOB
#define SPI2_GPIO_CLOCK     RCC_APB2Periph_GPIOB
#define SPI2_PIN_NSS        GPIO_Pin_12
#define SPI2_PIN_SCK        GPIO_Pin_13
#define SPI2_PIN_MISO       GPIO_Pin_14
#define SPI2_PIN_MOSI       GPIO_Pin_15


void SPI2Init(void);
uint16_t SPI2_SendAndRead(uint16_t data);
void SPI2_CS_LOW(void);
void SPI2_CS_HIGH(void);



#endif /* __SPI_H */


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

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "spi.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

void SPI2_CS_LOW(void)
{
    GPIO_ResetBits(GPIOB,GPIO_Pin_12);
}

void SPI2_CS_HIGH(void)
{
    GPIO_SetBits(GPIOB,GPIO_Pin_12);
}

uint16_t SPI2_SendAndRead(uint16_t data)
{
    SPI_I2S_SendData(SPI2, data);
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
    return SPI_I2S_ReceiveData(SPI2);
}


void SPI2Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;

    RCC_APB1PeriphClockCmd(SPI2_CLOCK, ENABLE);
    RCC_APB2PeriphClockCmd(SPI2_GPIO_CLOCK, ENABLE);

    GPIO_InitStructure.GPIO_Pin = SPI2_PIN_SCK | SPI2_PIN_MOSI;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(SPI2_GPIO_GROUP, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = SPI2_PIN_NSS;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(SPI2_GPIO_GROUP, &GPIO_InitStructure);


    GPIO_InitStructure.GPIO_Pin = SPI2_PIN_MISO;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(SPI2_GPIO_GROUP, &GPIO_InitStructure);

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI2, &SPI_InitStructure);

    GPIO_SetBits(GPIOB,GPIO_Pin_12);

    SPI_Cmd(SPI2, ENABLE);
}



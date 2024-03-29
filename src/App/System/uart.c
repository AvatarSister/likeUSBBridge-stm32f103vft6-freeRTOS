/**
  ******************************************************************************
  * @file    
  * @author  
  * @version 
  * @date    
  * @brief   
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include <string.h>
#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/* Private macro -------------------------------------------------------------*/
#define UART_TX_BUFF_LEN      512
#define UART_RX_BUFF_LEN      128
/* Private variables ---------------------------------------------------------*/
uint8_t g_u32UartTxBuf[UART_TX_BUFF_LEN];
uint32_t g_u32TxCnt;
uint32_t g_u32SentCnt;
uint8_t g_u32UartRxBuf[UART_RX_BUFF_LEN];
uint32_t g_u32RxCnt;

/* Private function prototypes -----------------------------------------------*/
void UART_SendByInterrupt(uint8_t *pBuff, uint32_t u32Len)
{
    if (u32Len > UART_TX_BUFF_LEN) u32Len = UART_TX_BUFF_LEN;
    memcpy(g_u32UartTxBuf, pBuff, u32Len);
    g_u32TxCnt = u32Len;
    g_u32SentCnt = 0;
    USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
}


/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
    USART_SendData(USART1, (uint8_t) ch);

    /* Loop until the end of transmission */
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
    {}

    return ch;
}

void UART_BufferInit(void)
{
    g_u32RxCnt = 0;
    g_u32SentCnt = 0;
    g_u32TxCnt = 0;
}

void UART_Configuration(void)
{
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    UART_BufferInit();

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO, ENABLE);

    /* Enable the USART1 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 14;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Configure USART1 Rx as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure USART1 Tx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* USARTy and USARTz configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
    */
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    /* Configure USART1 */
    USART_Init(USART1, &USART_InitStructure);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART1, ENABLE);
}

/**
  * @brief  
  * @param  None
  * @retval None
  */
void USART1_IRQHandler(void)
{
    
    if (USART_GetFlagStatus(USART1, USART_FLAG_PE) != RESET)
    {
        USART_ReceiveData(USART1);
        USART_ClearFlag(USART1, USART_FLAG_PE);
    }

    if (USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET)
    {
        USART_ReceiveData(USART1);
        USART_ClearFlag(USART1, USART_FLAG_ORE);
    }

    if (USART_GetFlagStatus(USART1, USART_FLAG_FE) != RESET)
    {
        USART_ReceiveData(USART1);
        USART_ClearFlag(USART1, USART_FLAG_FE);
    }

    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {   
        USART_ClearFlag(USART1, USART_FLAG_RXNE);
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
        g_u32UartRxBuf[g_u32RxCnt++] = USART_ReceiveData(USART1);
    }

    if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
    {
        /* Write one byte to the transmit data register */
        if (g_u32SentCnt < g_u32TxCnt)
        {
          USART_SendData(USART1, g_u32UartTxBuf[g_u32SentCnt++]);
        }
        else
        {
            USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
        }
    }
}

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

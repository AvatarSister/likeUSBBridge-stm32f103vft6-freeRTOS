/**
  ******************************************************************************
  * @file    main.c 
  * @author  LIKE
  * @version V1.0.0
  * @date    31-August-2018
  * @brief   Main program body
  ******************************************************************************
  * @attention
  ******************************************************************************
  */  

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "delay.h"
#include "uart.h"
#include "usb_port.h"
#include "usb_lib.h"
#include "usb_pwr.h"
#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
__IO uint8_t PrevXferComplete = 1;
__IO uint8_t usbReceived = 0;
uint8_t Send_Buffer[64];
const char welcome[20]= "hello world.\n";

volatile uint8_t touchInterrupFlag = FALSE;
/*******************************************************************************
* Function Name  : Set_USB_GPIO
* Description    : 
* Input          : None.
* Return         : None.
*******************************************************************************/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void vPowerOnForSIU2(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_SetBits(GPIOA,GPIO_Pin_0);
    GPIO_SetBits(GPIOA,GPIO_Pin_1);
    GPIO_SetBits(GPIOA,GPIO_Pin_2);
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void vTestGpio_Init(void)
{

 GPIO_InitTypeDef  GPIO_InitStructure;

 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);  //使能PB,PE端口时钟

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;               //LED0-->PB.5 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;        //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       //IO口速度为50MHz
 GPIO_Init(GPIOC, &GPIO_InitStructure);                  //根据设定参数初始化GPIOB.5
 GPIO_SetBits(GPIOC,GPIO_Pin_0);                         //PB.5 输出高
}
/**
  * @brief  .
  * @param  None
  * @retval None
  */
void vSetTestGpio(BaseType_t xData)
{
    if (xData == pdTRUE)
    {
        GPIO_SetBits(GPIOC,GPIO_Pin_0);
    }
    else if (xData == pdFALSE)
    {
        GPIO_ResetBits(GPIOC,GPIO_Pin_0);
    }
}
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void vTestTask (void *pvPrameters)
{
    uint32_t ulCnt = 0;
    for (;;)
    {
        if ((ulCnt & 0x03) == 0x03)
        {
            vSetTestGpio(pdFALSE);
        }
        else
        {
            vSetTestGpio(pdTRUE);
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
        printf(" %05d\n", ulCnt);
        if (touchInterrupFlag)
        {
            touchInterrupFlag = FALSE;
            printf("Touch Interrupt.\n");
        }
        ulCnt++;
    }
}

/**
  * @brief  Configure PB5 in interrupt mode
  * @param  None
  * @retval None
  */
void TouchInterruptInit(void)
{
    EXTI_InitTypeDef   EXTI_InitStructure;
    GPIO_InitTypeDef   GPIO_InitStructure;
    NVIC_InitTypeDef   NVIC_InitStructure;
    /* Enable GPIOA clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    /* Configure PA.00 pin as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* Enable AFIO clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource5);

    /* Configure EXTI0 line */
    EXTI_InitStructure.EXTI_Line = EXTI_Line5;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 12;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
    //SCB->VTOR 用于重定向vector, 做bootloader时必备
    vPowerOnForSIU2();
    vTestGpio_Init();
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    UART_Configuration();
    TouchInterruptInit();
    portUSBInit(13, 0);

    xTaskCreate(vTestTask, "my1stTask", 1000, NULL, 3, NULL);


    vTaskStartScheduler();          //开启任务调度


    for (;;);
#if 0
    /* Infinite loop */
    while (1)
    {
        
        if ((PrevXferComplete) && (bDeviceState == CONFIGURED) && (usbReceived))
        {
          /* Write the descriptor through the endpoint */    
          USB_SIL_Write(EP1_IN, (uint8_t*) Send_Buffer, 64);  
         
          SetEPTxValid(ENDP1);

          PrevXferComplete = 0;
          usbReceived = 0;
        }
        Delay(10);
    }
#endif
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

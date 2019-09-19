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
#include "spi.h"
#include "usb_port.h"
#include "usb_lib.h"
#include "usb_pwr.h"
#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"


#include "interface_spi.h"
#include "interface_usb.h"
#include "siu2.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
__IO uint8_t PrevXferComplete = 1;
__IO uint8_t usbReceived = 0;
uint8_t Send_Buffer[64];
int ReceivedLen;
const char welcome[20]= "hello world.\n";

volatile uint8_t touchInterrupFlag = FALSE;

uint8_t SPI_ReceiveBuffer[64];
uint16_t SPI_ReceiveCnt;


SemaphoreHandle_t xMutexPrintf;


void vToggleGPIOPin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    GPIO_WriteBit(GPIOx, GPIO_Pin, (BitAction)(1 - GPIO_ReadOutputDataBit(GPIOx, GPIO_Pin)));
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void vTestTask (void *pvPrameters)
{
#if 0
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
#else
    int ret;
    for (;;)
    {
        if (touchInterrupFlag)
        {
            touchInterrupFlag = FALSE;
            printf("Touch Interrupt.\n");
            ret = SPI_Read_BootId(SPI_ReceiveBuffer, 2);
            if (ret == SPI_OK)
            {
                printf("Boot id: %02X%02X\n",SPI_ReceiveBuffer[0],SPI_ReceiveBuffer[1]);
            }
            else
            {
                printf("Read boot id fail. [%d]\n",ret);
            }

        }
    }
    
#endif
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
  
int main(void)
{
    //SCB->VTOR 用于重定向vector, 做bootloader时必备
    SIU2_Init();
    Delay_Configuariton();
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    xMutexPrintf = xSemaphoreCreateMutex();
    vInterfaceUsbInit();
    
    UART_Configuration();
    SPI2Init();
    portUSBInit(13, 0);
    xTaskCreate(vTaskUsbInterface, "UsbInterface", 1000, NULL, 3, NULL);
    vTaskStartScheduler();          //开启任务调度
    for (;;);

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

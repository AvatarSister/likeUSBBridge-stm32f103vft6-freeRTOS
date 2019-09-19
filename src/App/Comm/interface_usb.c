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
#include <stdio.h>
#include <string.h>

#include "stm32f10x.h"
#include "usb_lib.h"
#include "usb_pwr.h"
#include "interface_spi.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
SemaphoreHandle_t xSemaphoreDeviceStateConfigured;

extern SemaphoreHandle_t xMutexPrintf;
extern __IO uint8_t PrevXferComplete;
extern __IO uint8_t usbReceived;
extern uint8_t Send_Buffer[];
extern uint8_t Receive_Buffer[];

/*------------------ ---------------------------------------------------------*/

void vSetDeviceStateConfiguredFromISR(void)
{
    BaseType_t xHigherPriorityTaskWoken;
    bDeviceState = CONFIGURED;
    xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(xSemaphoreDeviceStateConfigured, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void vInterfaceUsbInit(void)
{
    xSemaphoreDeviceStateConfigured = xSemaphoreCreateBinary();
}

void vTaskUsbInterface (void *pvPrameters)
{
    int ret;
    for (;;)
    {
        if (bDeviceState != CONFIGURED)
        {
            //xSemaphoreTake(xSemaphoreDeviceStateConfigured, portMAX_DELAY);
            if(xSemaphoreTake(xSemaphoreDeviceStateConfigured, pdMS_TO_TICKS(3000)) == pdFAIL)
            {
                xSemaphoreTake(xMutexPrintf, portMAX_DELAY);
                {
                    printf("Usb is not configured.\n");
                }
                xSemaphoreGive(xMutexPrintf);
            }
        }
        else
        {
            if ((PrevXferComplete) && (usbReceived))
            {
                memset(Send_Buffer,0,64);
                Send_Buffer[0] = 0x55;
                switch (Receive_Buffer[0])
                {
                    case 0x01:
                        download_fw();
                        break;
                    case 0x02:
                        ret = SPI_ReadRegs(Receive_Buffer[1],&Send_Buffer[1],1);
                        if (ret != SPI_OK)
                        {
                            xSemaphoreTake(xMutexPrintf, portMAX_DELAY);
                            {
                                printf("SPI error[%d]\n",ret);
                            }
                            xSemaphoreGive(xMutexPrintf);
                            Send_Buffer[0] = 0xAA;
                        }
                        break;
                    case 0x03:
                        SPI_WriteAndRead_Direct(Receive_Buffer[1],NULL,NULL,0);
                        Send_Buffer[0] = 0xAA;
                        Send_Buffer[1] = Receive_Buffer[1];
                        break;
                    case 0x04:
                        SPI_WriteAndRead_Direct(Receive_Buffer[1],&Receive_Buffer[2],NULL,1);
                        Send_Buffer[0] = 0xAA;
                        Send_Buffer[1] = Receive_Buffer[1];
                        Send_Buffer[2] = Receive_Buffer[2];
                        break;
                    default:
                        break;
                }

                
            

                USB_SIL_Write(EP1_IN, (uint8_t*) Send_Buffer, 64);
                SetEPTxValid(ENDP1);
                PrevXferComplete = 0;
                usbReceived = 0;
            }
            //vTaskDelay(pdMS_TO_TICKS(1));
        }

        
    }
}



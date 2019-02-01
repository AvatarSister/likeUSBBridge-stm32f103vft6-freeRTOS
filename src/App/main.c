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
#include "hw_config.h"
#include "usb_lib.h"
#include "usb_pwr.h"
#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
__IO uint8_t PrevXferComplete = 1;
__IO uint8_t usbReceived = 0;
uint8_t Send_Buffer[64];
/*******************************************************************************
* Function Name  : Set_USB_GPIO
* Description    : 
* Input          : None.
* Return         : None.
*******************************************************************************/
void Set_USB_GPIO(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;  
    /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32xxx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32xxx.c file
    */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    /********************************************/
    /*  Configure USB DM/DP pins                */
    /********************************************/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_DISCONNECT, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
}
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
    /*!< At this stage the microcontroller clock setting is already configured, 
    this is done through SystemInit() function which is called from startup
    file (startup_stm32f10x_xx.s) before to branch to application main.
    To reconfigure the default setting of SystemInit() function, refer to
    system_stm32f10x.c file
    */

    UART_Configuration();

    Delay_Configuariton();

    Set_USB_GPIO();

    USB_Interrupts_Config();

    Set_USBClock();

    USB_Init();

    Delay(1000);

    //UART_SendByInterrupt(au8Str, sizeof(au8Str));
    printf(" hello like!\n");

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

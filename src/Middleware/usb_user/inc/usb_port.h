/**
  ******************************************************************************
  * @file    usb_port.h
  * @author  like
  * @version 
  * @date    12-07-2019
  * @brief   usb Hardware Configuration & Setup
  ******************************************************************************
  * @attention
  *
  *
  ******************************************************************************
  */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_PORT_H
#define __USB_PORT_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "usb_type.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Define if Low power mode is enabled; it allows entering the device into 
   STOP mode following USB Suspend event, and wakes up after the USB wakeup
   event is received. */
//#define USB_LOW_PWR_MGMT_SUPPORT

/*Unique Devices IDs register set*/
#define         ID1          (0x1FFFF7E8)
#define         ID2          (0x1FFFF7EC)
#define         ID3          (0x1FFFF7F0)

#define RCC_APBxPeriph_GPIO_DM_DP           RCC_APB2Periph_GPIOA
#define GPIOx_DM_DP                         GPIOA
#define GPIO_PIN_x_DM                       GPIO_Pin_11
#define GPIO_PIN_x_DP                       GPIO_Pin_12


#define RCC_APB2Periph_GPIO_DISCONNECT      RCC_APB2Periph_GPIOD
#define GPIOx_DISCONNECT                    GPIOD
#define GPIO_PIN_x_DISCONNECT               GPIO_Pin_3

/* Exported define -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void Set_USBClock(void);
void Leave_LowPowerMode(void);
void USB_Interrupts_Config(uint8_t PreemptionPriotiry, uint8_t SubPriority);
void USB_Cable_Config (FunctionalState NewState);
void Get_SerialNum(void);
void Set_USB_GPIO(void);
void portUSBInit(uint8_t PreemptionPriotiry, uint8_t SubPriority);

#endif  /*__USB_PORT_H*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

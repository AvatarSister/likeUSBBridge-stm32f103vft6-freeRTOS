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
#ifndef __INTERFACE_USB_H
#define __INTERFACE_USB_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef true
#define true      1
#endif

#ifndef false
#define false     0
#endif
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */

void vTaskUsbInterface (void *pvPrameters);
void vInterfaceUsbInit(void);


#endif /* __INTERFACE_USB_H */


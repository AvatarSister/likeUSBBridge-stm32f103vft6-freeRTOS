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
#ifndef __INTERFACE_SPI_H
#define __INTERFACE_SPI_H

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
typedef enum
{
    SPI_OK = 0,
    SPI_BUSY,
    SPI_CMD_DATA_ERR,
    SPI_CMD_DATA_CRC_ERR
}ENUM_SPI_ERROR;

typedef enum
{
    SPI_WRITE = 0,
    SPI_READ
}ENUM_SPI_WRITE_READ;

typedef enum
{
    CRC_DISABLE = 0,
    CRC_ENABLE
}ENUM_SPI_CRC_EN;
/* Exported functions ------------------------------------------------------- */
int SPI_Read_BootId(uint8_t *pdata, int len);
int SPI_WriteRegs(uint8_t addr, uint8_t *pSendBuff, int len);
int SPI_ReadRegs(uint8_t addr, uint8_t *pReadBuff,int len);

void download_fw(void);


#endif /* __INTERFACE_SPI_H */


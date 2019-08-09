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
#include "interface_spi.h"
#include "delay.h"
#include "siu2.h"
#include <stdio.h>


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define DELAY_US_CS_LOW_TO_FIRST_BYTE          2
#define DELAY_US_FIRST_BYTE_TO_SENCOND         25
#define DELAY_US_LAST_BYTE_TO_CS_HIGH          2

/* Private variables ---------------------------------------------------------*/
#define AL2_FCS_COEF                     ((1 << 15) + (1 << 10) + (1 << 3))

const uint8_t fw[] = {
#include "fw_app.i"
};
uint16_t DrvCRC(uint16_t *ptr16, uint16_t length)
{
    uint16_t cFcs = 0;
    uint16_t i, j;
            
    for ( i = 0; i < length; i++ )
    {
        cFcs ^= *ptr16++;
        for (j = 0; j < 16; j ++)
        {
            if (cFcs & 0x01)
            {
                cFcs = (uint16_t)((cFcs >> 1) ^ AL2_FCS_COEF);
            }
            else
            {
                cFcs >>= 1;
            }
        }
    }
    return cFcs;
}

uint16_t ecc_cal_host(uint8_t *data, int data_len)
{
    uint16_t ecc = 0;
    uint16_t i = 0;
    uint16_t j = 0;
    uint16_t al2_fcs_coef = AL2_FCS_COEF;

    for (i = 0; i < data_len; i += 2 ) {
        ecc ^= ((data[i] << 8) | (data[i + 1]));
        
        for (j = 0; j < 16; j ++) {
            if (ecc & 0x01) {
                ecc = (u16)((ecc >> 1) ^ al2_fcs_coef);
            } else {
                ecc >>= 1;
            }
        }
    }
    return ecc;
}


uint16_t CRCKERMIT(const uint8_t * pDataIn, int iLenIn)
{     
    uint16_t i=0,j=0;
    uint16_t wCRC = 0xffff;
    for( i = 0; i < iLenIn; i++)
    {  
        wCRC ^=pDataIn[i];
        for(j = 0; j < 8; j++)
        {
            if(wCRC&0x01) wCRC=(wCRC>>1)^0X8408;
            else          wCRC=(wCRC>>1);
        }      
    }  
    return wCRC;
}



void SPI_WriteAndRead_Direct(uint8_t head, uint8_t *pSendData, uint8_t *pReadData, int len)
{
    int i;

    SPI2_CS_LOW();
    DelayUs(DELAY_US_CS_LOW_TO_FIRST_BYTE);
    SPI2_SendAndRead(head);
    DelayUs(DELAY_US_FIRST_BYTE_TO_SENCOND);

    if (pReadData == NULL)
    {
        if (pSendData == NULL)
        {
            for (i = 0; i < len; i++)
            {
                SPI2_SendAndRead(0x00);
            }
        }
        else
        {
            for (i = 0; i < len; i++)
            {
                SPI2_SendAndRead(pSendData[i]);
            }
        }

    }
    else
    {
        if (pSendData == NULL)
        {
            for (i = 0; i < len; i++)
            {
                pReadData[i] = SPI2_SendAndRead(0x00);
            }
        }
        else
        {
            for (i = 0; i < len; i++)
            {
                pReadData[i] = SPI2_SendAndRead(pSendData[i]);
            }
        }
    }
    DelayUs(DELAY_US_LAST_BYTE_TO_CS_HIGH);
    SPI2_CS_HIGH();
}

uint16_t StatusPacket(void)
{
    uint16_t status;
    uint8_t readBuff[2];
    readBuff[0]= 0xEA;
    SPI_WriteAndRead_Direct(0x05, NULL, readBuff, 1);
    status = readBuff[0];

    return status;
}

void ControlPacket(ENUM_SPI_WRITE_READ WorR, ENUM_SPI_CRC_EN CCRC, ENUM_SPI_CRC_EN DCRC, uint8_t *pdata, int len)
{
    int i;
    uint8_t WRCL;
    uint16_t crcResutl;
    uint8_t buffer[32];
    int send_len;

    WRCL = 0;
    if (WorR) WRCL |= 0x80;
    if (CCRC) WRCL |= 0x40;
    if (DCRC) WRCL |= 0x20;

    if (len > 15) len = 15;

    WRCL += len;

    buffer[0] = WRCL;
    for (i = 0; i < len; i++)
    {
        buffer[i+1] = pdata[i];
    }

    send_len = len + 1;

    if (CCRC)
    {
        crcResutl = CRCKERMIT(buffer, send_len);
        buffer[send_len] = (uint8_t)crcResutl;
        buffer[send_len+1] = (uint8_t)(crcResutl >> 8);
        send_len += 2;
    }

    SPI_WriteAndRead_Direct(0xC0, buffer, NULL, send_len);
}

void DataPacket(uint8_t *pSendData, uint8_t *pReadData, int len)
{
    SPI_WriteAndRead_Direct(0x3F, pSendData, pReadData, len);
}


int SPI_Wait_Idle(void)
{
    int maxRetryTime = 3;
    int retry;
    uint16_t status;

    retry = 0;
    while (true)
    {
        status = StatusPacket();
        //printf("%04X\n",status);

        if ((status & 0x0080))
        {
            retry++;
            if (retry == maxRetryTime) break;
        }
        else
        {
            break;
        }
        DelayUs(150);
    }

    if (retry == maxRetryTime)
    {
        return SPI_BUSY;
    }
    else
    {
        return SPI_OK;
    }
}


int SPI_Read_BootId(uint8_t *pdata, int len)
{
    int ret;
    uint8_t buff[2];

    ret = SPI_Wait_Idle();
    if (ret) return ret;
    DelayUs(150);

    buff[0] = 0x90;
    ControlPacket(SPI_READ, CRC_DISABLE, CRC_DISABLE, buff, 1);
    DelayUs(150);

    ret = SPI_Wait_Idle();
    if (ret) return ret;
    DelayUs(150);

    DataPacket(NULL, pdata, len);
    DelayUs(150);
    return SPI_OK;
}

int SPI_Write_Pram(uint8_t *pdata, int len)
{
    int ret;
    uint8_t buff[6];

    ret = SPI_Wait_Idle();
    if (ret) return ret;
    DelayUs(150);

    buff[0] = 0xAE;
    buff[1] = 0x00;
    buff[2] = 0x00;
    buff[3] = 0x00;
    buff[4] = (uint8_t)(len >> 8);
    buff[5] = (uint8_t)len;
    ControlPacket(SPI_WRITE, CRC_DISABLE, CRC_DISABLE, buff, 6);
    DelayUs(150);

    ret = SPI_Wait_Idle();
    if (ret) return ret;
    DelayUs(150);

    DataPacket(pdata, NULL, len);
    DelayUs(150);
    return SPI_OK;
}

int SPI_Remap(void)
{
    int ret;
    uint8_t pSendData[2];

    ret = SPI_Wait_Idle();
    if (ret) return ret;
    DelayUs(150);

    pSendData[0] = 0x01;
    pSendData[1] = 0x08;
    SPI_WriteAndRead_Direct(0xC0,pSendData,NULL,2);
    DelayUs(150);
    return SPI_OK;
}

int SPI_UpgradeCmd(void)
{
    int ret;
    uint8_t pSendData[2];

    ret = SPI_Wait_Idle();
    if (ret) return ret;
    DelayUs(150);

    pSendData[0] = 0x01;
    pSendData[1] = 0x55;
    SPI_WriteAndRead_Direct(0xC0,pSendData,NULL,2);
    DelayUs(150);
    return SPI_OK;
}

int SPI_StartCalEcc(int len)
{
    int ret;
    uint8_t buff[7];

    ret = SPI_Wait_Idle();
    if (ret) return ret;
    DelayUs(150);

    buff[0] = 0xCC;
    buff[1] = 0x00;
    buff[2] = 0x00;
    buff[3] = 0x00;
    buff[4] = (uint8_t)(len >> 16);
    buff[5] = (uint8_t)(len >> 8);
    buff[6] = (uint8_t)len;
    ControlPacket(SPI_WRITE, CRC_DISABLE, CRC_DISABLE, buff, 7);
    DelayUs(150);
    return SPI_OK;
}

int SPI_Read_Ecc(uint8_t *pdata, int len)
{
    int ret;
    uint8_t buff[2];

    ret = SPI_Wait_Idle();
    if (ret) return ret;
    DelayUs(150);

    buff[0] = 0xCD;
    ControlPacket(SPI_READ, CRC_DISABLE, CRC_DISABLE, buff, 1);
    DelayUs(150);

    ret = SPI_Wait_Idle();
    if (ret) return ret;
    DelayUs(150);

    DataPacket(NULL, pdata, len);
    DelayUs(150);
    return SPI_OK;
}

int SPI_WriteOrRead_Regs(ENUM_SPI_WRITE_READ WorR, uint8_t addr, uint8_t *pSendBuff, uint8_t *pReadBuff, int len)
{
    int ret;
    uint8_t buff[3];

    ret = SPI_Wait_Idle();
    if (ret) return ret;
    DelayUs(150);

    buff[0] = addr;
    buff[1] = (uint8_t)(len >> 8);
    buff[2] = (uint8_t)len;
    ControlPacket(WorR, CRC_DISABLE, CRC_DISABLE, buff, 3);
    DelayUs(150);

    ret = SPI_Wait_Idle();
    if (ret) return ret;
    DelayUs(150);

    DataPacket(pSendBuff, pReadBuff, len);
    DelayUs(150);
    return SPI_OK;
}

int SPI_WriteRegs(uint8_t addr, uint8_t *pSendBuff, int len)
{
    return SPI_WriteOrRead_Regs(SPI_WRITE, addr, pSendBuff, NULL, len);
}

int SPI_ReadRegs(uint8_t addr, uint8_t *pReadBuff,int len)
{
    return SPI_WriteOrRead_Regs(SPI_READ, addr, NULL, pReadBuff, len);
}

void vToggleGPIOPin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

void download_fw(void)
{
    uint8_t buff[2];
    uint16_t host_ecc,tp_ecc;
    int len;

    len = sizeof(fw);
    
    SET_TP_RESET_PIN(0);
    DelayUs(9000);
    SET_TP_RESET_PIN(1);
    DelayUs(15000);
    SPI_UpgradeCmd();
    DelayUs(10000);
    SPI_Read_BootId(buff,2);
    
    printf("BootId:0x%02X%02X\n",buff[0],buff[1]);
    SPI_Write_Pram((uint8_t *)fw, len);
    SPI_StartCalEcc(len);
    host_ecc = ecc_cal_host((uint8_t *)fw, len);
    printf("Host ecc:%d\n" , host_ecc);
    DelayUs(5000);
    SPI_Read_Ecc(buff,2);
    tp_ecc = (buff[0] << 8) + buff[1];
    printf("  tp ecc:%d\n" , tp_ecc);

    SPI_Remap();
    printf("Remap.\n");
}



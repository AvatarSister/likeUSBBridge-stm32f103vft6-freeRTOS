/**
  ******************************************************************************
  * @file    SysTick/TimeBase/main.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main program body.
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

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
  */
void DelayUs(__IO uint16_t nTime)
{
    TIM2->ARR = (nTime - 1);
    TIM2->PSC = 71;
    TIM2->EGR |= 0x0001;
    TIM_Cmd(TIM2,ENABLE);
    TIM_ClearFlag(TIM2,TIM_IT_Update);
    while (TIM_GetFlagStatus(TIM2,TIM_IT_Update) == RESET)
    {
    }
    TIM_ClearFlag(TIM2,TIM_IT_Update);
    //TIM_Cmd(TIM2,DISABLE);
}


/**
  * @brief  
  * @param  None
  * @retval None
  */
void Delay_Configuariton(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);

    /* Tout = ((arr+1)*(psc+1))/Tclk */
    TIM_TimeBaseStructure.TIM_Period = 1;
    TIM_TimeBaseStructure.TIM_Prescaler = 71;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上计数
    TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);

    TIM_SelectOnePulseMode(TIM2, TIM_OPMode_Single);
}

/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

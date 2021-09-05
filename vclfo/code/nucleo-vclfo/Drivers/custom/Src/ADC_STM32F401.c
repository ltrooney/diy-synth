/*
 * ADC_ex.c
 *
 *  Created on: Aug 21, 2021
 *      Author: lukerooney
 */

#include "tim.h"
#include "adc.h"
#include "../Inc/ADC_STM32F401.h"

uint16_t adc_val_0;
uint16_t adc_val_1;
uint16_t *adc_queue;

void ADC_Init() {
	adc_queue = &adc_val_0;

	// start ADC trigger timer
  HAL_TIM_Base_Start(&htim3);

  // start DMA
  ADC_Start_DMA_DBM(&hadc1, (uint32_t *) &adc_val_0, (uint32_t *) &adc_val_1, 100);
}


uint16_t ADC_queue_pop() {
	return *adc_queue;
}


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hdma) {
	if (adc_queue == &adc_val_0) {
		adc_queue = &adc_val_1;
	} else {
		adc_queue = &adc_val_0;
	}
}

/**
	* overwrites the HAL_ADC_Start() function to use double buffer mode
  */
HAL_StatusTypeDef ADC_Start_DMA_DBM(ADC_HandleTypeDef* hadc, uint32_t* pData, uint32_t* M1Data, uint32_t Length)
{
  __IO uint32_t counter = 0U;
  ADC_Common_TypeDef *tmpADC_Common;
  
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(hadc->Init.ContinuousConvMode));
  assert_param(IS_ADC_EXT_TRIG_EDGE(hadc->Init.ExternalTrigConvEdge)); 
  
  /* Process locked */
  __HAL_LOCK(hadc);
  
  /* Enable the ADC peripheral */
  /* Check if ADC peripheral is disabled in order to enable it and wait during 
  Tstab time the ADC's stabilization */
  if((hadc->Instance->CR2 & ADC_CR2_ADON) != ADC_CR2_ADON)
  {  
    /* Enable the Peripheral */
    __HAL_ADC_ENABLE(hadc);
    
    /* Delay for ADC stabilization time */
    /* Compute number of CPU cycles to wait for */
    counter = (ADC_STAB_DELAY_US * (SystemCoreClock / 1000000U));
    while(counter != 0U)
    {
      counter--;
    }
  }
  
  /* Check ADC DMA Mode                                                     */
  /* - disable the DMA Mode if it is already enabled                        */
  if((hadc->Instance->CR2 & ADC_CR2_DMA) == ADC_CR2_DMA)
  {
    CLEAR_BIT(hadc->Instance->CR2, ADC_CR2_DMA);
  }
  
  /* Start conversion if ADC is effectively enabled */
  if(HAL_IS_BIT_SET(hadc->Instance->CR2, ADC_CR2_ADON))
  {
    /* Set ADC state                                                          */
    /* - Clear state bitfield related to regular group conversion results     */
    /* - Set state bitfield related to regular group operation                */
    ADC_STATE_CLR_SET(hadc->State,
                      HAL_ADC_STATE_READY | HAL_ADC_STATE_REG_EOC | HAL_ADC_STATE_REG_OVR,
                      HAL_ADC_STATE_REG_BUSY);
    
    /* If conversions on group regular are also triggering group injected,    */
    /* update ADC state.                                                      */
    if (READ_BIT(hadc->Instance->CR1, ADC_CR1_JAUTO) != RESET)
    {
      ADC_STATE_CLR_SET(hadc->State, HAL_ADC_STATE_INJ_EOC, HAL_ADC_STATE_INJ_BUSY);  
    }
    
    /* State machine update: Check if an injected conversion is ongoing */
    if (HAL_IS_BIT_SET(hadc->State, HAL_ADC_STATE_INJ_BUSY))
    {
      /* Reset ADC error code fields related to conversions on group regular */
      CLEAR_BIT(hadc->ErrorCode, (HAL_ADC_ERROR_OVR | HAL_ADC_ERROR_DMA));         
    }
    else
    {
      /* Reset ADC all error code fields */
      ADC_CLEAR_ERRORCODE(hadc);
    }

    /* Process unlocked */
    /* Unlock before starting ADC conversions: in case of potential           */
    /* interruption, to let the process to ADC IRQ Handler.                   */
    __HAL_UNLOCK(hadc);   

    /* Pointer to the common control register to which is belonging hadc    */
    /* (Depending on STM32F4 product, there may be up to 3 ADCs and 1 common */
    /* control register)                                                    */
    tmpADC_Common = ADC_COMMON_REGISTER(hadc);

    /* Set the DMA transfer complete callback */
    // hadc->DMA_Handle->XferCpltCallback = ADC_DMAConvCplt;

    /* Set the DMA half transfer complete callback */
    // hadc->DMA_Handle->XferHalfCpltCallback = ADC_DMAHalfConvCplt;
    
    /* Set the DMA error callback */
    // hadc->DMA_Handle->XferErrorCallback = ADC_DMAError;

    
    /* Manage ADC and DMA start: ADC overrun interruption, DMA start, ADC     */
    /* start (in case of SW start):                                           */
    
    /* Clear regular group conversion flag and overrun flag */
    /* (To ensure of no unknown state from potential previous ADC operations) */
    __HAL_ADC_CLEAR_FLAG(hadc, ADC_FLAG_EOC | ADC_FLAG_OVR);

    /* Enable ADC overrun interrupt */
    __HAL_ADC_ENABLE_IT(hadc, ADC_IT_OVR);
    
    /* Enable ADC DMA mode */
    hadc->Instance->CR2 |= ADC_CR2_DMA;

     /* Enable ADC DSS */
    hadc->Instance->CR2 |= ADC_CR2_DDS;
    
    /* Start the DMA channel */
    HAL_DMAEx_MultiBufferStart(hadc->DMA_Handle, (uint32_t)&hadc->Instance->DR,(uint32_t)pData, (uint32_t)M1Data, Length);
    
    /* Check if Multimode enabled */
    if(HAL_IS_BIT_CLR(tmpADC_Common->CCR, ADC_CCR_MULTI))
    {
#if defined(ADC2) && defined(ADC3)
      if((hadc->Instance == ADC1) || ((hadc->Instance == ADC2) && ((ADC->CCR & ADC_CCR_MULTI_Msk) < ADC_CCR_MULTI_0)) \
                                  || ((hadc->Instance == ADC3) && ((ADC->CCR & ADC_CCR_MULTI_Msk) < ADC_CCR_MULTI_4)))
      {
#endif /* ADC2 || ADC3 */
        /* if no external trigger present enable software conversion of regular channels */
        if((hadc->Instance->CR2 & ADC_CR2_EXTEN) == RESET) 
        {
          /* Enable the selected ADC software conversion for regular group */
          hadc->Instance->CR2 |= (uint32_t)ADC_CR2_SWSTART;
        }
#if defined(ADC2) && defined(ADC3)
      }
#endif /* ADC2 || ADC3 */
    }
    else
    {
      /* if instance of handle correspond to ADC1 and  no external trigger present enable software conversion of regular channels */
      if((hadc->Instance == ADC1) && ((hadc->Instance->CR2 & ADC_CR2_EXTEN) == RESET))
      {
        /* Enable the selected ADC software conversion for regular group */
          hadc->Instance->CR2 |= (uint32_t)ADC_CR2_SWSTART;
      }
    }
  }
  else
  {
    /* Update ADC state machine to error */
    SET_BIT(hadc->State, HAL_ADC_STATE_ERROR_INTERNAL);

    /* Set ADC error code to ADC IP internal error */
    SET_BIT(hadc->ErrorCode, HAL_ADC_ERROR_INTERNAL);
  }
  
  /* Return function status */
  return HAL_OK;
}

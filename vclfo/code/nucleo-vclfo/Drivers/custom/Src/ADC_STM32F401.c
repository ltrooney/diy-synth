/*
 * ADC_ex.c
 *
 *  Created on: Aug 21, 2021
 *      Author: lukerooney
 */

#include "../Inc/ADC_STM32F401.h"

#define ADC_BUFF_SIZE 1

uint16_t adc_0_buff[ADC_BUFF_SIZE];

void ADC_Init(TIM_HandleTypeDef *htim, ADC_HandleTypeDef *hadc) {
	// enable the specified TIM peripheral
  HAL_TIM_Base_Start(htim);

  // enable DSS which allows DMA to start over upon each conversion
  hadc->Instance->CR2 |= ADC_CR2_DDS;

  // enable the ADC peripheral, 
  // configure the DMA stream hooked up to the ADC, 
  // then, enable the DMA peripheral
  HAL_ADC_Start_DMA(hadc, (uint32_t *) &adc_0_buff, 1);
}


uint16_t ADC_queue_pop() {
	return adc_0_buff[0];
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {}
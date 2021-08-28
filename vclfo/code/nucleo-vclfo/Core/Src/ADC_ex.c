/*
 * ADC_ex.c
 *
 *  Created on: Aug 21, 2021
 *      Author: lukerooney
 */

#include "ADC_ex.h"
#include "tim.h"
#include "adc.h"

uint16_t adc_val_0;
uint16_t adc_val_1;
uint16_t *adc_queue;

void ADC_Init() {
	adc_queue = &adc_val_0;
  // /* Enable the double buffer mode */
  // hdma_adc1.Instance->CR |= (uint32_t)DMA_SxCR_DBM;

  //  /* Configure DMA Stream destination address */
  // hdma_adc1.Instance->M1AR = (uint32_t) &adc_val_1;
  
  // start DMA
  HAL_ADC_Start_DMA(&hadc1, (uint32_t *) &adc_val_0, 1);
}


uint16_t ADC_queue_pop() {
	return adc_val_0;
}


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hdma) {
	HAL_ADC_Start_DMA(&hadc1, (uint32_t *) &adc_val_0, 1);

	// if (adc_queue == &adc_val_0) {
	// 	adc_queue = &adc_val_1;
	// } else {
	// 	adc_queue = &adc_val_0;
	// }
}

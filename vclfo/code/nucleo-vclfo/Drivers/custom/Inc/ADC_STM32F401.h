/*
 * ADC_ex.h
 *
 *  Created on: Aug 21, 2021
 *      Author: lukerooney
 */

#ifndef INC_ADC_EX_H_
#define INC_ADC_EX_H_

#include "main.h"

void ADC_Init(TIM_HandleTypeDef*, ADC_HandleTypeDef*);

uint16_t ADC_queue_pop();

HAL_StatusTypeDef ADC_Start_DMA_DBM(ADC_HandleTypeDef*, uint32_t*, uint32_t*, uint32_t);

#endif /* INC_ADC_EX_H_ */

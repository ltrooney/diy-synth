/*
 * DAC.h
 *
 *  Created on: Aug 15, 2021
 *      Author: lukerooney
 */

#ifndef INC_DAC_H_
#define INC_DAC_H_

#include "main.h"

void DAC_Init(TIM_HandleTypeDef*, SPI_HandleTypeDef*, DMA_HandleTypeDef*);

void DAC_queue_push(float);

void DAC_xfer_M0_cplt(DMA_HandleTypeDef *);
void DAC_xfer_M1_cplt(DMA_HandleTypeDef *);


#endif /* INC_DAC_H_ */

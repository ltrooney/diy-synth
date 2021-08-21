/*
 * DAC.h
 *
 *  Created on: Aug 15, 2021
 *      Author: lukerooney
 */

#ifndef INC_DAC_H_
#define INC_DAC_H_

#include "main.h"

#define DAC_QUEUE_NUM_SAMPLES 64
#define DAC_MEM_BUF_SIZE 3 * DAC_QUEUE_NUM_SAMPLES

void DAC_Init();

void DAC_queue_push(float);

static inline uint16_t float2DACsample14(float f) {
	const uint16_t twotothethirteen = 8192;
	uint16_t result = (uint16_t) (f * (twotothethirteen-1)) + twotothethirteen;
	return result << 2;
}

void DAC_xfer_complete_callback(DMA_HandleTypeDef *);

#endif /* INC_DAC_H_ */

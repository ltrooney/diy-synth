/*
 * DAC.C
 *
 *  Created on: Aug 15, 2021
 *      Author: lukerooney
 */

#include "DAC.h"
#include "tim.h"
#include "spi.h"


uint8_t mem0_buf[DAC_MEM_BUF_SIZE];
uint8_t mem1_buf[DAC_MEM_BUF_SIZE];

uint8_t *sample_queue;
uint16_t dac_queue_nbytes;

void DAC_Init() {

	// initialize dummy DAC buffer
	for (int i = 0; i < DAC_MEM_BUF_SIZE; i++) {
		mem0_buf[i] = 0xAA;
		mem1_buf[i] = 0x00;
	}

	sample_queue = (uint8_t *) mem0_buf;

	HAL_DMA_RegisterCallback(&hdma_tim1_ch1_ch2_ch3, HAL_DMA_XFER_CPLT_CB_ID, DAC_xfer_complete_callback);

	// setup double buffer mode and enable the DMA stream
	HAL_DMAEx_MultiBufferStart(&hdma_tim1_ch1_ch2_ch3, (uint32_t) mem0_buf, (uint32_t) &hspi2.Instance->DR, (uint32_t) mem1_buf, DAC_MEM_BUF_SIZE);

	// enable SPI
	__HAL_SPI_ENABLE(&hspi2);
}

// converts float to a 14-bit DAC sample
void DAC_queue_push(float sample) {
	// check if queue has room for another sample
	if (dac_queue_nbytes + 3 > DAC_MEM_BUF_SIZE) {
		return;
	}

	// convert from float to 14-bit
	uint16_t sample_int = float2DACsample14(sample);

	// TODO: calculate 8 control bits
	uint8_t ctrl = 0;

	// add to queue
	sample_queue[dac_queue_nbytes] = ctrl;
	sample_queue[dac_queue_nbytes+1] = (uint8_t) (sample_int >> 8);
	sample_queue[dac_queue_nbytes+2] = (uint8_t) sample_int;

	dac_queue_nbytes += 3;
}


void DAC_xfer_complete_callback(DMA_HandleTypeDef *hdma) {
	if (sample_queue == (uint8_t *) mem0_buf) {
		sample_queue = (uint8_t *) mem1_buf;
	} else {
		sample_queue = (uint8_t *) mem0_buf;
	}
	dac_queue_nbytes = 0;
}

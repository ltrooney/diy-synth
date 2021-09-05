/*
 * DAC.C
 *
 *  Created on: Aug 15, 2021
 *      Author: lukerooney
 */

#include "tim.h"
#include "spi.h"
#include "../Inc/DAC_TI8164.h"


uint8_t M0_buff[DAC_MEM_BUFF_SIZE];
uint8_t M1_buff[DAC_MEM_BUFF_SIZE];

uint8_t *sample_queue;
uint16_t dac_queue_size_bytes;

void DAC_Init() {

	// zero-fill the DAC buffers
	for (int i = 0; i < DAC_MEM_BUFF_SIZE; i++) {
		M0_buff[i] = 0x00;
		M1_buff[i] = 0xFF;
	}

	sample_queue = (uint8_t *) M1_buff;

	// enable DMA request for TIM1_CH1,2,3,4
  __HAL_TIM_ENABLE_DMA(&htim1, TIM_DMA_CC1);
  __HAL_TIM_ENABLE_DMA(&htim1, TIM_DMA_CC2);
  __HAL_TIM_ENABLE_DMA(&htim1, TIM_DMA_CC3);
  __HAL_TIM_ENABLE_DMA(&htim1, TIM_DMA_CC4);

	// register DMA xfer complete and error callbacks for DMA stream hooked up to TIM1_CH1,2,3
	HAL_DMA_RegisterCallback(&hdma_tim1_ch1_ch2_ch3, HAL_DMA_XFER_CPLT_CB_ID, DAC_xfer_M0_cplt);
	HAL_DMA_RegisterCallback(&hdma_tim1_ch1_ch2_ch3, HAL_DMA_XFER_M1CPLT_CB_ID, DAC_xfer_M1_cplt);
	HAL_DMA_RegisterCallback(&hdma_tim1_ch1_ch2_ch3, HAL_DMA_XFER_ERROR_CB_ID, TIM_DMAError);


	// enable DBM and configure the DMA addresses and NDTR for stream hooked up to TIM1_CH1,2,3
	// then enable the DMA peripheral
	HAL_DMAEx_MultiBufferStart_IT(&hdma_tim1_ch1_ch2_ch3, (uint32_t) M0_buff, (uint32_t) &hspi2.Instance->DR, (uint32_t) M1_buff, DAC_MEM_BUFF_SIZE);

	// enable output compare channels TIM_CH1,2,3,4
	// then enable the TIM1 peripheral
  HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_2);
  HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_3);
  HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_4);

	// enable the SPI2 peripheral
	__HAL_SPI_ENABLE(&hspi2);
}

// converts float to a 14-bit DAC sample
void DAC_queue_push(float sample) {
	// check if queue has room for another sample
	if (dac_queue_size_bytes + BYTES_PER_DAC_SAMPLE > DAC_MEM_BUFF_SIZE) {
		return;
	}

	// convert from float to 14-bit
	uint16_t sample_int = float2DACsample14(sample);

	// TODO: calculate 8 control bits
	uint8_t ctrl = 0;

	// add to queue
	sample_queue[dac_queue_size_bytes] = ctrl;
	sample_queue[dac_queue_size_bytes+1] = (uint8_t) (sample_int >> 8);
	sample_queue[dac_queue_size_bytes+2] = (uint8_t) sample_int;

	dac_queue_size_bytes += BYTES_PER_DAC_SAMPLE;
}


// swap the sample_queue memory pointer and reset queue size
void DAC_xfer_M0_cplt(DMA_HandleTypeDef *hdma) {
	sample_queue = (uint8_t *) M0_buff;
	dac_queue_size_bytes = 0;
}

// swap the sample_queue memory pointer and reset queue size
void DAC_xfer_M1_cplt(DMA_HandleTypeDef *hdma) {
	sample_queue = (uint8_t *) M1_buff;
	dac_queue_size_bytes = 0;
}

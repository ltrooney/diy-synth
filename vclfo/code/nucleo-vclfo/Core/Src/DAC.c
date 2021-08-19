/*
 * DAC.C
 *
 *  Created on: Aug 15, 2021
 *      Author: lukerooney
 */

#include "DAC.h"
#include "main.h"
#include "tim.h"
#include "spi.h"

#define DAC_BUF_SIZE 192

uint8_t mem0_buf[DAC_BUF_SIZE];
uint8_t mem1_buf[DAC_BUF_SIZE];

void DAC_Init() {

	// initialize dummy DAC buffer
	for (int i = 0; i < DAC_BUF_SIZE; i++) {
		mem0_buf[i] = 1;
		mem1_buf[i] = 1;
	}

	// setup double buffer mode and enable the DMA stream
	tim1_DMA_DBM_start((uint32_t) mem0_buf, (uint32_t) &hspi2.Instance->DR, (uint32_t) mem1_buf, DAC_BUF_SIZE);

	__HAL_SPI_ENABLE(&hspi2);
}

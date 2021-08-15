/*
 * vclfo.c
 *
 *  Created on: Aug 15, 2021
 *      Author: lukerooney
 */


static const int f = 440;
static const float wave_shape = 0;
static int playback_rate;
static float y2;
static float k;

uint8_t dummy_dac_buf[] = {1,1,1,1,1,1,1,1,1,1};

static int tim10_val;

void vclfo_loop() {

	// start timer
	HAL_TIM_Base_Start(&htim10);
	  tim10_val = __HAL_TIM_GET_COUNTER(&htim10);

	  k = 0;


	  /* USER CODE BEGIN WHILE */
	  while (1)
	  {
		  /* Check if the SPI is already enabled */
		  if ((hspi2.Instance->CR1 & SPI_CR1_SPE) != SPI_CR1_SPE) {
			  /* Enable SPI peripheral */
			  __HAL_SPI_ENABLE(&hspi2);
		  }

		  /* Enable Tx DMA Request */
		  hspi2.Instance->CR2 |= SPI_CR2_TXDMAEN;
		  HAL_Delay(500);
	//	  if (__HAL_TIM_GET_COUNTER(&htim10) - tim10_val >= 1) {
	////		  HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
	//
	//		  playback_rate = SMPL_SIZE * f / FS;
	//		  int wave_shape_floor = (int) wave_shape;
	//		  GPIOA->BSRR = GPIO_PIN_4; // gpio on
	//		  y2 = wt_sample(wave_shape_floor, f, k);
	//		  GPIOA->BSRR = (uint32_t) GPIO_PIN_4 << 16U;	// gpio off
	//
	//		  k = fmodf(k + playback_rate, SMPL_SIZE);
	//
	//		  tim10_val = __HAL_TIM_GET_COUNTER(&htim10);
	//	  }

		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	  }

	return;
}

/*
 * utility.c
 *
 *  Created on: Sep 5, 2021
 *      Author: lukerooney
 */

#include "utility.h"

float translate_range(float x, float from_min, float from_max, float to_min, float to_max) {
	float from_rng = from_max - from_min;
	float to_rng = to_max - to_min;

	if (from_rng == 0) {
		return 0;
	}

	float out = (((x - from_min) * to_rng) / from_rng) + to_min;
	return clamp(out, to_min, to_max);
}

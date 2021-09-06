/*
 * utility.h
 *
 *  Created on: Sep 5, 2021
 *      Author: lukerooney
 */

#ifndef CUSTOM_UTILITY_H_
#define CUSTOM_UTILITY_H_

static inline int min(int a, int b) { return ((a<b)?a:b); }

static inline int max(int a, int b) { return ((a<b)?b:a); }

static inline float interp(float x1, float x2, float amount) {
	return (1-amount) * x1 + amount * x2;
}

static inline float clamp(float x, float lower, float upper) {
	return x <= lower ? lower : x >= upper ? upper : x;
}

static inline float lerp2pt(float x0, float y0, float x1, float y1, float k) {
	return (y0 * (x1 - k) + y1 * (k - x0)) / (x1 - x0);
}

float translate_range(float, float, float, float, float);

#endif /* CUSTOM_UTILITY_H_ */

/*
 * engine.h
 *
 *  Created on: Aug 15, 2021
 *      Author: lukerooney
 */

#ifndef INC_ENGINE_H_
#define INC_ENGINE_H_

static inline int min(int, int);
static inline int max(int, int);
static inline float interp(float, float, float);
static inline float clamp(float, float, float);
static float translate_range(float, float, float, float, float);
static inline float lookup(const int, const int, const int);
static inline float lerp2pt(float, float, float, float, float);
static float wt_sample(const int, const float, const float);


#endif /* INC_ENGINE_H_ */

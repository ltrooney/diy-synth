/*
 * engine.h
 *
 *  Created on: Aug 15, 2021
 *      Author: lukerooney
 */

#ifndef INC_ENGINE_H_
#define INC_ENGINE_H_

#define SMPL_SIZE 256
#define NUM_OCTAVES 10
#define FS 48000
#define NUM_WAVES 4
#define VCO_F_MIN 40
#define VCO_F_MAX 20000

static const float wavetable[10240] = {
#include "../../../wavetable_init.dat"
};

typedef struct
{
	float f;
	float wave_idx;
	uint8_t mode; // 0 = vco, 1 = lfo
	// ...
} CV_inputs;

float lookup(const int, const int, const int);
float wt_sample(CV_inputs*);

#endif /* INC_ENGINE_H_ */

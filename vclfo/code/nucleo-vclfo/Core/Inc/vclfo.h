/*
 * vclfo.h
 *
 *  Created on: Aug 15, 2021
 *      Author: lukerooney
 */

#ifndef INC_VCLFO_H_
#define INC_VCLFO_H_

static const int SMPL_SIZE = 256;
static const int NUM_OCTAVES = 10;
static const int FS = 48000;
static const int NUM_WAVES = 4;
static const int VCO_F_MIN = 40;
static const int VCO_F_MAX = 20000;
static const float wavetable[10240] = {
#include "../../../wavetable_init.dat"
};

void vclfo_start();

#endif /* INC_VCLFO_H_ */

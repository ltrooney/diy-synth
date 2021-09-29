/*
 * engine.c
 *
 *  Created on: Aug 15, 2021
 *      Author: lukerooney
 */

#include "engine.h"
#include "math.h"
#include "utility.h"

static float k;

float lookup(const int wave_idx, const int octave_idx, const int sample_idx) {
	return wavetable[SMPL_SIZE * ((NUM_OCTAVES * wave_idx) + octave_idx) + sample_idx];
}

float wt_sample(CV_inputs *cv) {
  k = fmodf(k + (SMPL_SIZE * cv->f / FS), SMPL_SIZE);

	const float f_clamp = clamp(cv->f, VCO_F_MIN, VCO_F_MAX);

	// quick compute log2(f_clamp / VCO_F_MIN)
	int intlog = 0;
	int x = (int) f_clamp / VCO_F_MIN;
	while (x >>= 1) ++intlog;

	const int oct1_idx = intlog + 1;
	const int oct2_idx = min(oct1_idx + 1, NUM_OCTAVES - 1);

	const int x0 = (int) k;
	const int x1 = x0 + 1;

	const int p_lo = VCO_F_MIN << (oct1_idx - 1);

	const int interpol_amt = translate_range(f_clamp, p_lo, p_lo << 1, 0, 1);

	// interpolate between octaves at point x0
	const float px0_1 = lookup(cv->wave_idx, oct1_idx, x0 % SMPL_SIZE);
	const float px0_2 = lookup(cv->wave_idx, oct2_idx, x0 % SMPL_SIZE);
	const float y0 = interp(px0_1, px0_2, interpol_amt);

	// interpolate between octaves at point x1
	const float px1_1 = lookup(cv->wave_idx, oct1_idx, x1 % SMPL_SIZE);
	const float px1_2 = lookup(cv->wave_idx, oct2_idx, x1 % SMPL_SIZE);
	const float y1 = interp(px1_1, px1_2, interpol_amt);

	return lerp2pt(x0, y0, x1, y1, k);
}

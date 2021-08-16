/*
 * engine.c
 *
 *  Created on: Aug 15, 2021
 *      Author: lukerooney
 */

/*
static inline int min(int a, int b) { return ((a<b)?a:b); }

static inline int max(int a, int b) { return ((a<b)?b:a); }

static inline float interp(float x1, float x2, float amount) {
	return (1-amount) * x1 + amount * x2;
}

static inline float clamp(float x, float lower, float upper) {
	return x <= lower ? lower : x >= upper ? upper : x;
}

static float translate_range(float x, float from_min, float from_max, float to_min, float to_max) {
	float from_rng = from_max - from_min;
	float to_rng = to_max - to_min;

	if (from_rng == 0) {
		return 0;
	}

	float out = (((x - from_min) * to_rng) / from_rng) + to_min;
	return clamp(out, to_min, to_max);
}

static inline float lookup(const int wave_idx, const int octave_idx, const int sample_idx) {
	return wavetable[SMPL_SIZE * ((NUM_OCTAVES * wave_idx) + octave_idx) + sample_idx];
}

static inline float lerp2pt(float x0, float y0, float x1, float y1, float k) {
	return (y0 * (x1 - k) + y1 * (k - x0)) / (x1 - x0);
}


static float wt_sample(const int wave_idx, const float f, const float k) {

	const float f_clamp = clamp(f, VCO_F_MIN, VCO_F_MAX);

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
	const float px0_1 = lookup(wave_idx, oct1_idx, x0 % SMPL_SIZE);
	const float px0_2 = lookup(wave_idx, oct2_idx, x0 % SMPL_SIZE);
	const float y0 = interp(px0_1, px0_2, interpol_amt);

	// interpolate between octaves at point x1
	const float px1_1 = lookup(wave_idx, oct1_idx, x1 % SMPL_SIZE);
	const float px1_2 = lookup(wave_idx, oct2_idx, x1 % SMPL_SIZE);
	const float y1 = interp(px1_1, px1_2, interpol_amt);

	const float y = lerp2pt(x0, y0, x1, y1, k);

	return y;
}
*/

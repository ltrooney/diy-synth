#include <stdio.h>
#include <math.h>
#include <stdbool.h>

struct WAV_header
{
    // canonical 44 byte header format for WAV (RIFF) files

    // RIFF chunk descriptor, 12 bytes
    u_int32_t chunk_id;         // ='RIFF'
    u_int32_t chunk_size;       // size of the rest of the header and data
    u_int32_t format;           // ='WAVE'

    // 'fmt' sub-chunk, 24 bytes
    u_int32_t subchunk1_id;     // ='fmt '
    u_int32_t subchunk1_size;   // size of rest of subchunk
    u_int16_t audio_fmt;        // 1=PCM, other=compression
    u_int16_t num_channels;     // 1=mono, 2=stereo, etc.
    u_int32_t sample_rate;        
    u_int32_t byte_rate;        // # bytes per second
    u_int16_t block_align;      // # bytes per sample across all channels
    u_int16_t bits_per_sample;  // # bits per sample

    // 'data' sub-chunk, 8 bytes
    u_int32_t subchunk2_id;     // ='data'
    u_int32_t subchunk2_size;   // # data bytes
};

void gen_wave_period_sine(float *period_buf, u_int32_t num_samples);
void gen_wave_period_square(float *period_buf, u_int32_t num_samples);
void gen_wave_period_saw(float *period_buf, u_int32_t num_samples);
void plot(char *fname, float *period_buf, u_int32_t num_samples);
void wav_init(struct WAV_header *header, int16_t *data, u_int32_t data_size);
bool wav_gen(char *fname, float *wave_period, u_int32_t num_samples, u_int32_t f, u_int32_t sample_rate, float dur);

const u_int32_t SAMP_RATE = 44100;
const u_int32_t NUM_SAMPLES = 256;
const u_int8_t BITS_PER_SAMPLE = 16;
const float VOLUME = 0.3;   // 0=no sound, 1=full sound


int main() {

    float wave_period[NUM_SAMPLES];

    gen_wave_period_square(wave_period, NUM_SAMPLES);

    plot("test.dat", wave_period, NUM_SAMPLES);
    
    wav_gen("./sine.wav", wave_period, NUM_SAMPLES, 440, SAMP_RATE, 3.0);

    return 0;
}

// generates 1 period of a sine wave with range [-1,1]
void gen_wave_period_sine(float *period_buf, u_int32_t num_samples) {
    float phase_inc = 2 * M_PI / num_samples;
    float rads = 0;
    for (int i = 0; i < num_samples; i++) {
        period_buf[i] = sin(rads);
        rads += phase_inc;
    }
}

// generates 1 period of a square wave with range [-1,1]
void gen_wave_period_square(float *period_buf, u_int32_t num_samples) {
    for (int i = 0; i < num_samples; i++) {
        if (i >= (num_samples / 2)) {
            period_buf[i] = 1;
        } else {
            period_buf[i] = -1;
        }
    }
}

// generates 1 period of a saw wave with range [-1,1]
void gen_wave_period_saw(float *period_buf, u_int32_t num_samples) {
    float running = -1;
    const float inc = (float) (2.0 / num_samples);
    for (int i = 1; i < num_samples; i++) {
        float new = running + inc;
        period_buf[i] = new;
        running = new;
    }
}

// outputs wave period buffer to a .dat file
void plot(char *fname, float *period_buf, u_int32_t num_samples) {
    FILE *output_file;
    output_file = fopen(fname, "wb");

    if (output_file == NULL) {
        printf("Cannot open file");
        return;
    }
    
    for (int i = 0; i < num_samples; i++) {
        fprintf(output_file, "%i\t%f\n", i, period_buf[i]);
    }
    fclose(output_file);
}

void wav_init(struct WAV_header *header, int16_t *data, u_int32_t data_size) {
    header->chunk_id = 0x46464952; // 'RIFF' in little-endian
    header->format = 0x45564157;   // 'WAVE' in little-endian
    header->subchunk1_id = 0x20746d66;  // 'fmt' in little-endian
    header->subchunk1_size = 16;
    header->audio_fmt = 1;
    header->num_channels = 1;
    header->sample_rate = SAMP_RATE;
    header->bits_per_sample = BITS_PER_SAMPLE;
    header->byte_rate = header->sample_rate * header->num_channels * header->bits_per_sample / 8;
    header->block_align = header->num_channels * header->bits_per_sample / 8;
    header->subchunk2_id = 0x61746164;  // 'data' in little-endian
    header->subchunk2_size = data_size;
    header->chunk_size = 36 + header->subchunk2_size;
}

bool wav_gen(char *fname, float *wave_period, u_int32_t num_samples, u_int32_t f, u_int32_t sample_rate, float dur) {
    struct WAV_header header;

    FILE *output_file;
    output_file = fopen(fname, "wb");

    if (output_file == NULL) {
        printf("Cannot open file");
        return false;
    }

    const u_int32_t data_size = sample_rate * dur;

    int16_t output[data_size];
    int smps_per_pd = SAMP_RATE / f;

    for (int i = 0; i < data_size; i++) {
        // map i from range [0, smps_per_pd] into [0, num_samples]
        int wave_t_idx = (((float) i) / smps_per_pd) * num_samples;
        float samp = wave_period[wave_t_idx % num_samples];
        samp *= (1 << (BITS_PER_SAMPLE - 1));
        samp *= VOLUME;
        output[i] = (int16_t) samp;

        // // find nearest x in wave_period --> x1
        // // map i from range [0, data_size] to [0, num_samples]
        // int x1 = (((float) i) / num_x) * num_samples;
        // float y1 = wave_period[x1];

        // // let (x2,y2) <-- point following (x1,y1)
        // int x2 = x1 + 1;
        // float y2 = wave_period[x2];

        // // calculate m
        // float m = (y2 - y1) / (x2 - x1);

        // // calculate beta
        // float beta = m;

    }

    // generate and write the header
    wav_init(&header, output, data_size);
    fwrite(&header, sizeof(struct WAV_header), 1, output_file);

    // write data to the file
    fwrite(output, sizeof(int16_t), data_size, output_file);

    fclose(output_file);
    return true;
}
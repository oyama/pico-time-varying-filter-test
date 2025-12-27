#include <math.h>

static float w1 = 0.0f;
static float w2 = 0.0f;

void filter_df2_reset(void) {
    w1 = 0.0f; w2 = 0.0f;
}

void filter_df2_process(float cutoff, float Q, float *in, float *out, size_t size) {
    float fs = 48000.0f;
    float w0 = 2.0f * M_PI * cutoff / fs;
    float alpha = sinf(w0) / (2.0f * Q);

    float b0 = (1.0f - cosf(w0)) / 2.0f;
    float b1 = 1.0f - cosf(w0);
    float b2 = (1.0f - cosf(w0)) / 2.0f;
    float a0 = 1.0f + alpha;
    float a1 = -2.0f * cosf(w0);
    float a2 = 1.0f - alpha;

    b0 /= a0; b1 /= a0; b2 /= a0; a1 /= a0; a2 /= a0;

    for (size_t n = 0; n < size; ++n) {
        float wn = in[n] - a1 * w1 - a2 * w2;
        out[n] = b0 * wn + b1 * w1 + b2 * w2;
        w2 = w1;
        w1 = wn;
    }
}

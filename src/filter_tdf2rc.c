#include <math.h>

static float s1 = 0.0f;
static float s2 = 0.0f;

void filter_tdf2rc_reset(void) {
    s1 = 0.0f; s2 = 0.0f;
}

void filter_tdf2rc_process(float cutoff, float Q, float *in, float *out, size_t size) {
    float fs = 48000.0f;
    float w0 = 2.0f * M_PI * cutoff / fs;
    float alpha = sinf(w0) / (2.0f * Q);

    float b0 = (1.0f - cosf(w0)) / 2.0f;
    float b1 = 1.0f - cosf(w0);
    float b2 = (1.0f - cosf(w0)) / 2.0f;
    float a0 = 1.0f + alpha;
    float a1 = -2.0f * cosf(w0);
    float a2 = 1.0f - alpha;

    float nb0 = b0 / a0, nb1 = b1 / a0, nb2 = b2 / a0;
    float na1 = a1 / a0, na2 = a2 / a0;

    for (size_t n = 0; n < size; ++n) {
        float yn = nb0 * in[n] + s1;

        float next_s1 = nb1 * in[n] - na1 * yn + s2;
        float next_s2 = nb2 * in[n] - na2 * yn;
        s1 = next_s1;
        s2 = next_s2;
        out[n] = yn;
    }
}

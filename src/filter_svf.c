#include <math.h>

static float s1 = 0.0f;
static float s2 = 0.0f;

void filter_svf_reset(void) {
    s1 = 0.0f;
    s2 = 0.0f;
}

void filter_svf_process(float cutoff, float Q, float *in, float *out, size_t size) {
    float fs = 48000.0f;
    float g = tanf(M_PI * cutoff / fs);
    float R = 1.0f / (2.0f * Q);
    float h = 1.0f / (g * g + 2.0f * R * g + 1.0f);

    for (size_t n = 0; n < size; ++n) {
        float un = in[n];
        float x1 = (g * h) * un + (h * s1) - (g * h * s2);
        float x2 = (g * g * h) * un + (g * h * s1) + (h * (1.0f + 2.0f * R * g) * s2);

        s1 = s1 + 2.0f * g * (un - 2.0f * R * x1 - x2);
        s2 = s2 + 2.0f * g * x1;

        out[n] = x2;
    }
}

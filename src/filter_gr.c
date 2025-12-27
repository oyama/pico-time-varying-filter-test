#include <math.h>

static float x1 = 0.0f;
static float x2 = 0.0f;

void filter_gr_reset(void) {
    x1 = 0.0f; x2 = 0.0f;
}

void filter_gr_process(float cutoff, float Q, float *in, float *out, size_t size) {
    float fs = 48000.0f;
    float w0 = 2.0f * M_PI * cutoff / fs;
    float r = expf(-w0 / (2.0f * Q)); 
    float theta = w0;

    float a = r * cosf(theta);
    float b = r * sinf(theta);

    // (1 - 2r cos(theta) + r^2) / sin(theta)
    float gain_scale = (1.0f - 2.0f * a + r * r) / (b + 1e-20f);

    for (size_t n = 0; n < size; ++n) {
        float un = in[n];

        float next_x1 = a * x1 - b * x2 + un;
        float next_x2 = b * x1 + a * x2;

        x1 = next_x1;
        x2 = next_x2;

        out[n] = x2 * gain_scale; 
    }
}

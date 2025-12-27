#include <math.h>
#include <stdio.h>

#include "arm_math.h"
#include "pico/stdlib.h"

// --- Common filter interface ---
typedef void (*filter_func)(float cutoff, float res, float *in, float *out, size_t size);
typedef void (*reset_func)(void);

typedef struct {
    const char *name;
    filter_func process;
    reset_func reset;
} filter_entry_t;

// Declare filter functions implemented in external files
extern void filter_svf_process(float, float, float *, float *, size_t);
extern void filter_svf_reset(void);
extern void filter_gr_process(float, float, float *, float *, size_t);
extern void filter_gr_reset(void);
extern void filter_df2_process(float, float, float *, float *, size_t);
extern void filter_df2_reset(void);
extern void filter_tdf2rc_process(float, float, float *, float *, size_t);
extern void filter_tdf2rc_reset(void);

const filter_entry_t filter_registry[] = {
    {"DF2", filter_df2_process, filter_df2_reset},  // Direct-Form II
    {"GR", filter_gr_process, filter_gr_reset},     // Coupled Form
    {"SVF", filter_svf_process, filter_svf_reset},  // State Variable Filter
    {"TDF2RC", filter_tdf2rc_process, filter_tdf2rc_reset},  // Stabilized TDF-II
};

const size_t num_filters = sizeof(filter_registry) / sizeof(filter_entry_t);

// --- Test settings ---
#define FS 48000.0f
#define SAMPLES 4096  // about 85 ms
#define WARMUP 16000  // warm-up period to let the system reach steady state
#define CHANGE_PT 2048
#define TEST_FREQ 100.0f

static float32_t in_buf[SAMPLES + WARMUP];
static float32_t out_actual[SAMPLES], out_ref[SAMPLES], diff_buf[SAMPLES];
static float32_t fft_mag[SAMPLES / 2], dummy_buf[WARMUP];
static arm_rfft_fast_instance_f32 fft_inst;

void run_test(const filter_entry_t *filter, bool is_dc) {
    float32_t dc_val = 1.0f;
    for (int i = 0; i < SAMPLES + WARMUP; i++) {
        in_buf[i] = is_dc ? dc_val : sinf(2.0f * PI * TEST_FREQ * i / FS);
    }

    // --- 1. Generate measured output (Actual) ---
    filter->reset();
    // Step A: warm up at the initial parameter (80 Hz) to reach steady state
    filter->process(80.0f, 6.0f, in_buf, dummy_buf, WARMUP);
    // Step B: switch parameters to 120 Hz and measure
    filter->process(120.0f, 6.0f, &in_buf[WARMUP], out_actual, SAMPLES);

    // --- 2. Generate ideal output (Ideal: Output Switching Model) ---
    // Create the "steady-state response at 120 Hz"
    filter->reset();
    // Step A: warm up using the target parameter (120 Hz)
    filter->process(120.0f, 6.0f, in_buf, dummy_buf, WARMUP);
    // Step B: measure without changing anything
    filter->process(120.0f, 6.0f, &in_buf[WARMUP], out_ref, SAMPLES);

    // Compute the difference
    arm_sub_f32(out_actual, out_ref, diff_buf, SAMPLES);

    if (is_dc) {
        // --- Table 4: DC stimulus L2 norm ---
        float32_t energy;
        arm_dot_prod_f32(diff_buf, diff_buf, SAMPLES, &energy);
        printf("%-10s | ", filter->name);
        if (energy <= 1e-12f)
            printf("-inf\n");
        else
            printf("%6.2f\n", 10.0f * log10f(energy));
    } else {
        // --- Transient signal analysis ---
        float32_t var, sb_rms;
        arm_var_f32(diff_buf, SAMPLES, &var);

        // Apply an 85 ms Hann window
        for (int i = 0; i < SAMPLES; i++) {
            float32_t win = 0.5f * (1.0f - cosf(2.0f * PI * i / (SAMPLES - 1)));
            out_actual[i] *= win;
        }

        // Sideband analysis (remove 1 ERB around 100 Hz)
        arm_rfft_fast_f32(&fft_inst, out_actual, in_buf, 0);
        arm_cmplx_mag_f32(in_buf, fft_mag, SAMPLES / 2);
        for (int b = 6; b <= 12; b++)
            fft_mag[b] = 0;
        arm_rms_f32(fft_mag, SAMPLES / 2, &sb_rms);

        printf("%-10s | %-25.8f | %.8f\n", filter->name, var, sb_rms);
    }
}

int main() {
    stdio_init_all();
    sleep_ms(3000);
    arm_rfft_fast_init_f32(&fft_inst, SAMPLES);

    printf("\nObjective test results with DC stimulus (l2 norm dB)\n");
    printf("(Lower values are better, -inf is ideal)\n");
    printf("----------------------------------------------------------\n");
    for (size_t i = 0; i < num_filters; i++)
        run_test(&filter_registry[i], true);

    printf("\nTransient Signal Analysis (Perceptual Indicators)\n");
    printf("(Sideband energy correlation with MUSHRA: r = -0.59)\n");
    printf("----------------------------------------------------------------------------\n");
    printf("Structure  | Transient Signal Variance | Sideband energy (RMS)\n");
    printf("----------------------------------------------------------------------------\n");
    for (size_t i = 0; i < num_filters; i++)
        run_test(&filter_registry[i], false);

    while (1)
        tight_loop_contents();
}

/*
 * Task 1: The Golden Measure on the PC
 *
 * Integer square root of a 32-bit unsigned input x: the largest integer
 * whose square does not exceed x.  Golden version uses double precision
 * arithmetic and the standard library square root.
 *
 * Prediction (written before running):
 *   On a ~3 GHz x86-64 CPU, sqrt + floor + conversion is roughly
 *   20..40 instructions.  Estimate: 40 instructions * 0.33 ns/instr
 *   ~= 13 ns per call.  One call alone cannot be timed reliably because
 *   clock_gettime resolution is ~10..50 ns and scheduling noise is larger
 *   than the call itself, so we loop and divide.
 *
 * Build:   gcc -O2 -o golden_measure golden_measure.c -lm
 * Run:     ./golden_measure
 */

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <math.h>
#include <time.h>

#define TEST_INPUT 987654321u

static const uint32_t inputs[10] = {
    0, 1, 15, 16, 4095, 65535,
    123456789, 987654321, 4294836225u, 4294967295u
};

static uint32_t golden_isqrt(uint32_t x)
{
    return (uint32_t)floor(sqrt((double)x));
}

static double timestamp_us(void)
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	double timestamp = (double)ts.tv_sec * 1e6 + (double)ts.tv_nsec / 1e3;

	return timestamp;
}

/* Hand check: r^2 <= x < (r+1)^2, written out in full. */
static int hand_check(uint32_t x, uint32_t r)
{
	double r_sq = (double)r * (double)r;
	double r_plus1_sq = (double)(r+1) * (double)(r+1);

	int ok = (r_sq <= (double)x) &&(r_plus1_sq > (double)x);

	printf("x = %10" PRIu32 " r = %6" PRIu32 " r^2 = %10.0f" PRIu32 " (r+1)^2 = %10.0f  -> %s\n",
			x, r, r_sq, r_plus1_sq, ok ? "valid" : "INVALID");

	return ok;
}

static double time_n_calls(long reps)
{
	volatile uint32_t sink;
	double start_us, end_us;

	start_us = timestamp_us();
	for (long i = 0; i < reps; i++) {
		sink = golden_isqrt(TEST_INPUT);
	}
	end_us = timestamp_us();

	/* Return time per call in nanoseconds */
	double time_n_calls = ((end_us - start_us) * 1e3) / (double)reps;
	return time_n_calls;
}

int main(void)
{

	 /* Output table + hand check for all ten golden inputs. */
	 printf("Output table and hand-check for all ten inputs:\n");
	 for (int i = 0; i < 10; i++)
	 {
		 uint32_t r = golden_isqrt(inputs[i]);
	     hand_check(inputs[i], r);
	 }
	 printf("\n");

	 /* Timing: two runs with different repetition counts. */
	 long   reps1 = 1000000L;
	 long   reps2 = 10000000L;
	 double t1 = time_n_calls(reps1);
	 double t2 = time_n_calls(reps2);
	 double mean   = (t1 + t2) / 2.0;
	 double spread = fabs(t1 - t2);

	    printf("Run 1: %ld reps, %.3f ns/call\n", reps1, t1);
	    printf("Run 2: %ld reps, %.3f ns/call\n", reps2, t2);
	    printf("Mean: %.3f ns/call \nSpread: %.3f ns\n", mean, spread);

	    /* Golden output for the Task 2 question input. */
	    printf("\ngolden_isqrt(987654321) = %" PRIu32 "\n\n", golden_isqrt(987654321u));

	    return 0;
}
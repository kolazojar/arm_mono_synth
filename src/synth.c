/**
 * @file synth.c
 * @author Matt Krol
 */

#include <stdint.h>
#include "synth.h"
#include "lut.h"
#include "hardware.h"

void osc_init (struct osc *wave, uint32_t p, uint32_t f)
{
    wave->phase = p;
    wave->fctrl = f;
}

double sin_ptoa(struct osc *wave)
{
    uint32_t y1_index = wave->phase >> FRAC_BITS;
    uint32_t y2_index = y1_index + 1;
    uint32_t qtest= y1_index >> SIN_LUT_BITS;
    uint32_t frac = wave->phase & FRAC_MASK;
    int32_t y;
    /* calculate the correct index values */
    if (y2_index & (1 << (SIN_PHASE_BITS - 2))) {
        y2_index = ~y2_index;
    }
    y2_index &= SIN_LUT_MAX;
    if (qtest & 1) {
        y1_index = ~y1_index;
    }
    y1_index &= SIN_LUT_MAX;
    /* perform interpolation between table values using fixed point math */
    y = SIN_LUT[y1_index] + (int32_t)(((int64_t)frac*(SIN_LUT[y2_index] - SIN_LUT[y1_index])) >> FRAC_BITS);
    if (qtest > 1) {
        y = -y;
    }
    /* increment phase */
    wave->phase += wave->fctrl;
    /* scale to double */
    return ((double)y*SIN_LUT_SCALE);
}

uint16_t render_sample(void)
{
    return ((uint16_t)((1 << DAC_BITS) - 1)/2.0*(1.0 + sin_ptoa(&voice1)));
}

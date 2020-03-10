/**
 * @file synth.h
 * @author Matt Krol
 */

#ifndef SYNTH_H
#define SYNTH_H

#define ACC_BITS 32
#define SIN_LUT_BITS 8
#define SIN_LUT_MAX ((1 << SIN_LUT_BITS) - 1)
#define SIN_PHASE_BITS (SIN_LUT_BITS + 2)
#define SIN_PHASE_MAX ((1 << SIN_PHASE_BITS) - 1)
#define DAC_BITS 12
#define FRAC_BITS (ACC_BITS - SIN_PHASE_BITS)
#define FRAC_MASK ~(SIN_PHASE_MAX << FRAC_BITS)

struct osc {
    uint32_t phase;
    uint32_t fctrl;
};

struct osc voice1;

void osc_init(struct osc *wave, uint32_t p, uint32_t f);

double sin_ptoa(struct osc *wave);

uint16_t render_sample(void);

#endif

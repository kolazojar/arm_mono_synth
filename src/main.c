/**
 * @file src/main.c
 * @author Matt Krol
 */

#include "stm32f4xx.h"
#include "hardware.h"
#include "synth.h"

int main(void)
{
    pll_init();
    audio_init();
    /* init oscillator to 440 Hz sin wave */
    osc_init(&voice1, 0, 39370534);
    while (1) {
    }
    return 0;
}

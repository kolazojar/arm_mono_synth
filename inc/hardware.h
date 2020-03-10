/**
 * @file hardware.h
 * @author Matt Krol
 */

#ifndef HARDWARE_H
#define HARDWARE_H

#include <stdint.h>
#include "stm32f4xx.h"

#define AUDIO_BUFFER_SIZE ((uint16_t)256)

uint16_t audio_buffer[AUDIO_BUFFER_SIZE];

void pll_init(void);

void audio_init(void);

#endif

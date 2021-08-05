#pragma once

#ifndef I2S_H__
#define I2S_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "boards.h"
#include <nrf.h>

#include "nrf_drv_i2s.h"
#include "nrf_delay.h"

#include "sounds.h"

	// I2S configuration
#define PIN_MCK    (13) // no wire
#define PIN_SCK    (14) // p0.14 bordovii
#define PIN_LRCK   (15) // p0.15 wire blue
#define PIN_SDOUT  (16) // p0.16 wire red
#define PAUSE_TIME          3000

#define	NRF_I2S_STOP NRF_I2S->TASKS_START = 0;

void I2S_init();
void sound_cuica_start();
void sound_claves_start();

#ifdef __cplusplus
}
#endif

#endif /* I2C_H__ */

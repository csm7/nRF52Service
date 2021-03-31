#pragma once
#include <stdint.h>
#include <stdbool.h>

#include "app_timer.h"
#include "ble_types.h"
//#include "bsp.h"

#include "common_var.h"

#define ACELEROMETR_MEAS_INTERVAL APP_TIMER_TICKS(400) /**< Acelerometr level measurement interval (ticks). */

static void acelerometr_level_meas_timeout_handler(void* p_context);


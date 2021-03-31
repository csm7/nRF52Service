#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "bsp_config.h"
#include "bsp.h"


static uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID;                        /**< Handle of the current connection. */

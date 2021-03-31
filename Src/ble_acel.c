
#include <string.h>
#include "sdk_common.h"
#include "ble_srv_common.h"
#include "app_error.h"
#include "ble_acel.h"


/**@brief Function for handling the Connect event.
 *
 * @param[in]   p_cus       Custom Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_connect(ble_acel_t * p_cus, ble_evt_t const * p_ble_evt)
{
	UNUSED_PARAMETER(p_ble_evt);

	p_cus->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;

	ble_acel_evt_t evt;

	evt.evt_type = BLE_ACEL_EVT_CONNECTED;

	p_cus->evt_handler(p_cus, &evt);

}


/**@brief Function for handling the Disconnect event.
 *
 * @param[in]   p_cus       Custom service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_disconnect(ble_acel_t * p_cus, ble_evt_t const * p_ble_evt)
{
	UNUSED_PARAMETER(p_ble_evt);

	p_cus->conn_handle = BLE_CONN_HANDLE_INVALID; 
      
	ble_acel_evt_t evt;

	evt.evt_type = BLE_ACEL_EVT_DISCONNECTED;

	p_cus->evt_handler(p_cus, &evt);

}


/**@brief Function for handling the Write event.
 *
 * @param[in]   p_cus       Custom service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_write(ble_acel_t * p_cus, ble_evt_t const * p_ble_evt)
{
	ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
	ble_acel_evt_t                 evt;

	// writing to the command characteristic
 if(p_evt_write->handle == p_cus->command_value_handles.value_handle)
	{ 
		evt.params_command.command_data.p_data = p_evt_write->data;
		evt.params_command.command_data.length = p_evt_write->len;
		evt.evt_type = BLE_ACEL_EVT_COMMAND_RX; 

		p_cus->evt_handler(p_cus, &evt);
	}

	// writing to the temperature characteristic
 if((p_evt_write->handle == p_cus->acelerometr_value_handles.cccd_handle)
      && (p_evt_write->len == 2)
     )
	{
		// CCCD written, call application event handler
		if(p_cus->evt_handler != NULL)
		{

			if (ble_srv_is_notification_enabled(p_evt_write->data))
			{
				evt.evt_type = BLE_ACEL_NOTIFICATION_ENABLED;
			}
			else
			{
				evt.evt_type = BLE_ACEL_NOTIFICATION_DISABLED;
			}
			// Call the application event handler.
			p_cus->evt_handler(p_cus, &evt);
		}
	}
}


/**@brief Function for handling the Custom servie ble events.
 *
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
void ble_cus_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context)
{
	ble_acel_t * p_cus = (ble_acel_t *) p_context;

	switch (p_ble_evt->header.evt_id)
	{
	case BLE_GAP_EVT_CONNECTED:
		on_connect(p_cus, p_ble_evt);
		break;

	case BLE_GAP_EVT_DISCONNECTED:
		on_disconnect(p_cus, p_ble_evt);
		break;

	case BLE_GATTS_EVT_WRITE:
		on_write(p_cus, p_ble_evt);
		break;

	default:
		// No implementation needed.
		break;
	}
}


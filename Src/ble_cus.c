#include <stdlib.h>
/* 
 * ble_cus.c : file for the Custom ble service.
 * Author : Abdelali Boussetta  @rmptxf
 * For @Paarth
 */

#include <string.h>
#include "sdk_common.h"
#include "ble_srv_common.h"
#include "app_error.h"
#include "ble_cus.h"


/**@brief Function for handling the Connect event.
 *
 * @param[in]   p_cus       Custom Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_connect(ble_cus_t * p_cus, ble_evt_t const * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);

    p_cus->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;

    ble_cus_evt_t evt;

    evt.evt_type = BLE_CUS_EVT_CONNECTED;

    p_cus->evt_handler(p_cus, &evt);

}

/**@brief Function for handling the Disconnect event.
 *
 * @param[in]   p_cus       Custom service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_disconnect(ble_cus_t * p_cus, ble_evt_t const * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);

    p_cus->conn_handle = BLE_CONN_HANDLE_INVALID; 
      
    ble_cus_evt_t evt;

    evt.evt_type = BLE_CUS_EVT_DISCONNECTED;

    p_cus->evt_handler(p_cus, &evt);

}


/**@brief Function for handling the Write event.
 *
 * @param[in]   p_cus       Custom service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_write(ble_cus_t * p_cus, ble_evt_t const * p_ble_evt)
{
    ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
    ble_cus_evt_t                 evt;

    // writing to the command characteristic
   if ( p_evt_write->handle == p_cus->command_value_handles.value_handle)
    { 
        evt.params_command.command_data.p_data = p_evt_write->data;
        evt.params_command.command_data.length = p_evt_write->len;
        evt.evt_type = BLE_CUS_EVT_COMMAND_RX; 

        p_cus->evt_handler(p_cus, &evt);
    }

    // writing to the temperature characteristic
   if ((p_evt_write->handle == p_cus->temperature_value_handles.cccd_handle)
        && (p_evt_write->len == 2)
       )
    {
        // CCCD written, call application event handler
        if (p_cus->evt_handler != NULL)
        {

            if (ble_srv_is_notification_enabled(p_evt_write->data))
            {
	            enableNotificationAcel = 1;
                evt.evt_type = BLE_TEMP_NOTIFICATION_ENABLED;
            }
            else
            {
//	            enableNotificationAcel = 0;
                evt.evt_type = BLE_TEMP_NOTIFICATION_DISABLED;
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
void ble_cus_on_ble_evt( ble_evt_t const * p_ble_evt, void * p_context)
{
    ble_cus_t * p_cus = (ble_cus_t *) p_context;

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

/**@brief Function for adding the Temperature characteristic.
 *
 * @param[in]   p_cus        Custom service structure.
 * @param[in]   p_cus_init   Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t temperature_char_add(ble_cus_t * p_cus, const ble_cus_init_t * p_cus_init)
{

    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;

    uint8_t char_len = 4;
    uint8_t init_value[4] = {0x12, 0x34, 0x56, 0x78};

    // Add Custom Value characteristic
    memset(&cccd_md, 0, sizeof(cccd_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm); 
    
    cccd_md.write_perm = p_cus_init->temperature_char_attr_md.cccd_write_perm;
    cccd_md.vloc       = BLE_GATTS_VLOC_STACK;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 1;  
    char_md.char_props.write  = 0; 
    char_md.char_props.notify = 1; 
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = &cccd_md; 
    char_md.p_sccd_md         = NULL;
		        
    ble_uuid.type = p_cus->uuid_type;
    ble_uuid.uuid = TEMPERATURE_CHAR_UUID;

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_cus_init->temperature_char_attr_md.read_perm;
    attr_md.write_perm = p_cus_init->temperature_char_attr_md.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = char_len;
    attr_char_value.max_len   = char_len;
    attr_char_value.p_value   = init_value;

    err_code = sd_ble_gatts_characteristic_add(p_cus->service_handle, 
                                               &char_md,
                                               &attr_char_value,
                                               &p_cus->temperature_value_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}

/**@brief Function for adding the Command characteristic.
 *
 * @param[in]   p_cus        Custom service structure.
 * @param[in]   p_cus_init   Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t command_char_add(ble_cus_t * p_cus, const ble_cus_init_t * p_cus_init)
{

    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    uint8_t char_len = 1;
    uint8_t init_value[1] = {0};

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 0;
    char_md.char_props.write  = 1;        
    char_md.char_props.notify = 0; 
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = NULL; 
    char_md.p_sccd_md         = NULL;

    ble_uuid.type = p_cus->uuid_type;
    ble_uuid.uuid = COMMAND_CHAR_UUID;

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_cus_init->command_char_attr_md.read_perm;
    attr_md.write_perm = p_cus_init->command_char_attr_md.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = char_len;
    attr_char_value.max_len   = char_len;
    attr_char_value.p_value   = init_value;

    err_code = sd_ble_gatts_characteristic_add(p_cus->service_handle, 
                                               &char_md,
                                               &attr_char_value,
                                               &p_cus->command_value_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;

}


/**@brief Function for initializing the Custom ble service.
 *
 * @param[in]   p_cus       Custom service structure.
 * @param[in]   p_cus_init  Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */

uint32_t ble_cus_init(ble_cus_t * p_cus, ble_cus_t *p_acel, const ble_cus_init_t * p_cus_init, const ble_cus_init_t *p_acel_init)
{
    uint32_t   err_code;
    ble_uuid_t ble_uuid;
	  ble_uuid_t acel_ble_uuid;

#pragma region CUS_SERVICE_UUID_BASE
    // Initializing the  service structure
    p_cus->evt_handler               = p_cus_init->evt_handler;
    p_cus->conn_handle               = BLE_CONN_HANDLE_INVALID;

	// Add the Custom bel Service UUID
	ble_uuid128_t base_uuid = CUS_SERVICE_UUID_BASE;
    err_code =  sd_ble_uuid_vs_add(&base_uuid, &p_cus->uuid_type);
    VERIFY_SUCCESS(err_code);
    
    ble_uuid.type = p_cus->uuid_type;
    ble_uuid.uuid = CUS_SERVICE_UUID;

    // Add the Service to the database
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_cus->service_handle);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Add the temperature characteristic
   err_code =  temperature_char_add(p_cus, p_cus_init);
   APP_ERROR_CHECK(err_code);

    // Add the command characteristic
   err_code =  command_char_add(p_cus, p_cus_init);
   APP_ERROR_CHECK(err_code);				  
#pragma endregion //CUS_SERVICE_UUID_BASE

//--------------------------------------------------------------------------
    // Initializing the  service structure
    p_acel->evt_handler        = p_acel_init->evt_handler;
	  p_acel->conn_handle        = BLE_CONN_HANDLE_INVALID;

	// Add the Custom bel Service UUID
	ble_uuid128_t acel_base_uuid = ACEL_SERVICE_UUID_BASE;
	err_code =  sd_ble_uuid_vs_add(&acel_base_uuid, &p_acel->uuid_type);
	VERIFY_SUCCESS(err_code);
    
	acel_ble_uuid.type = p_acel->uuid_type;
	acel_ble_uuid.uuid = ACEL_SERVICE_UUID;

	// Add the Service to the database
	err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &acel_ble_uuid, &p_acel->service_handle);
	if (err_code != NRF_SUCCESS)
	{
		return err_code;
	}


	// Add the temperature characteristic
 err_code =  temperature_char_add(p_acel, p_acel_init);
	APP_ERROR_CHECK(err_code);

	// Add the command characteristic
 err_code =  command_char_add(p_acel, p_acel_init);
	APP_ERROR_CHECK(err_code);				  

   return NRF_SUCCESS;

}

/**@brief Function for updating the temperature data.
 *
 * @note 
 *       
 * @param[in]   p_cus          Custom Service structure.
 * @param[in]   p_data         data array
 * @param[in]   p_length       length
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */

uint32_t temperature_value_update(ble_cus_t * p_cus, uint8_t  * p_data, uint16_t  p_length)
{
    uint32_t   err_code;

    if (p_cus == NULL)
    {                                                                                          
        return NRF_ERROR_NULL;
    }

    ble_gatts_value_t   gatts_value;
    ble_cus_evt_t               evt;

    // Initialize value struct.
    memset(&gatts_value, 0, sizeof(gatts_value));

    gatts_value.len     = p_length;
    gatts_value.offset  = 0;
    gatts_value.p_value = p_data;

    // Update database.
    err_code = sd_ble_gatts_value_set(p_cus->conn_handle,
                                      p_cus->temperature_value_handles.value_handle,
                                      &gatts_value);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

	// Notify the host, if connected and notifications are enabled.
	//    if ((p_cus->conn_handle != BLE_CONN_HANDLE_INVALID) 
	//         && (evt.evt_type == BLE_TEMP_NOTIFICATION_ENABLED))
	  if(evt.evt_type == BLE_TEMP_NOTIFICATION_ENABLED)
	  {
		  if ((p_cus->conn_handle != BLE_CONN_HANDLE_INVALID)) 
		  {
			  ble_gatts_hvx_params_t hvx_params;

			  memset(&hvx_params, 0, sizeof(hvx_params));
			  //	    rand();
			  hvx_params.handle = p_cus->temperature_value_handles.value_handle;
			  hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
			  hvx_params.offset = gatts_value.offset;
			  hvx_params.p_len  = &gatts_value.len;
			  hvx_params.p_data = gatts_value.p_value;

			  err_code = sd_ble_gatts_hvx(p_cus->conn_handle, &hvx_params);
			  // Make sure the above function reports a BLE_GATTS_EVT_HVN_TX_COMPLETE
		  }
		  else
		  {
			  err_code = NRF_ERROR_INVALID_STATE;
		  }
		}

    return err_code;

}

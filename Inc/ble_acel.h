#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"

#define BLE_ACEL_BLE_OBSERVER_PRIO 2

/**@brief   Macro for defining a ble_acel instance.
 *
 * @param   _name   Name of the instance.
 * @hideinitializer
 */
 #define BLE_ACEL_DEF(_name)          \
 static ble_acel_t _name;             \
 NRF_SDH_BLE_OBSERVER(_name ## _obs,  \
 BLE_ACEL_BLE_OBSERVER_PRIO,           \
 ble_acel_on_ble_evt, &_name)

#define ACEL_SERVICE_UUID_BASE       { 0x23, 0xD1, 0x13, 0xEF, 0x5F, 0x78, 0x23, 0x15, \
                                       0xDE, 0xEF, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00 }

#define ACEL_SERVICE_UUID              0x1400
#define ACELX_CHAR_UUID                0x1401 
#define ACELY_CHAR_UUID                0x1402	
#define ACELZ_CHAR_UUID                0x1403	

/**@brief Acelerometr service event types.
*
*/
typedef enum
{      
	BLE_ACEL_EVT_CONNECTED, 
	BLE_ACEL_EVT_DISCONNECTED,       
	BLE_ACEL_NOTIFICATION_ENABLED,
	BLE_ACEL_NOTIFICATION_DISABLED,
	BLE_ACEL_EVT_COMMAND_RX,  

} ble_acel_evt_type_t;

/*
 * @details This structure is passed to an event when @ref BLE_ACEL_EVT_COMMAND_RX occurs.
 */
typedef struct 
{
	uint8_t const * p_data;   
	uint16_t   length;  
} command_data_t;

/**@brief Acelerometr Service event. */
typedef struct
{
	ble_acel_evt_type_t evt_type;                     

	union
	{
		command_data_t command_data;
       
	} params_command;

} ble_acel_evt_t;

// Forward declaration of the ble_cus_t type.
typedef struct ble_acel_s ble_acel_t;


/**@brief Acelerometr service event handler type. */
typedef void(*ble_acel_evt_handler_t)(ble_acel_t * p_acel, ble_acel_evt_t * p_evt);


/**@brief Acelerometr Service init structure. This contains all options and data needed for
 *        initialization of the service.*/
typedef struct
{
	ble_acel_evt_handler_t         evt_handler;                        /**< Event handler to be called for handling events in the Custom Service. */  
   
	ble_srv_cccd_security_mode_t  acelerometr_char_attr_md;      /**< Initial security level for the temperature characteristic attribute */
	ble_srv_cccd_security_mode_t  command_char_attr_md;          /**< Initial security level for the command characteristic attribute */

} ble_acel_init_t;


/**@brief Acelerometr Service structure. This contains various status information for the service. */
struct ble_acel_s
{
	ble_acel_evt_handler_t         evt_handler;                    /**< Event handler to be called for handling events in the Custom Service. */
	uint16_t                      service_handle;                 /**< Handle of Custom Service (as provided by the BLE stack). */
   
	ble_gatts_char_handles_t      acelerometr_value_handles;      /**< Handles related to the temperature characteristic. */
	ble_gatts_char_handles_t      command_value_handles;          /**< Handles related to the command characteristic. */
     
	uint16_t                      conn_handle;                    /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection). */
	uint8_t                       uuid_type; 
};


/**@brief Function for handling the Acelerometr servie ble events.
 *
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
void ble_acel_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context);


/**@brief Function for initializing the Acelerometr ble service.
 *
 * @param[in]   p_cus       Acelerometr service structure.
 * @param[in]   p_cus_init  Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */

uint32_t ble_acel_init(ble_acel_t * p_acel, const ble_acel_init_t * p_acel_init);


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

uint32_t acelerometr_value_update(ble_acel_t * p_cus, uint8_t  * p_data, uint16_t  p_length);
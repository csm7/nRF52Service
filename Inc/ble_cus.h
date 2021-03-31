/* 
 * ble_cus.h : file for the Custom ble service.
 * Author : Abdelali Boussetta  @rmptxf
 * For @Paarth
 */

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"


#define BLE_CUS_BLE_OBSERVER_PRIO  2

/**@brief   Macro for defining a ble_cus instance.
 *
 * @param   _name   Name of the instance.
 * @hideinitializer
 */
#define BLE_CUS_DEF(_name)                                                            \
static ble_cus_t _name;                                                               \
NRF_SDH_BLE_OBSERVER(_name ## _obs,                                                   \
                     BLE_CUS_BLE_OBSERVER_PRIO,                                       \
                     ble_cus_on_ble_evt, &_name)


#define CUS_SERVICE_UUID_BASE       { 0x23, 0xD1, 0x13, 0xEF, 0x5F, 0x78, 0x23, 0x15, \
                                      0xDE, 0xEF, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00 }

#define CUS_SERVICE_UUID              0x1200
#define TEMPERATURE_CHAR_UUID         0x1201 
#define COMMAND_CHAR_UUID             0x1202	
																		
/**@brief Custom service event types.
*
*/
typedef enum
{      
    BLE_CUS_EVT_CONNECTED, 
    BLE_CUS_EVT_DISCONNECTED,       
    BLE_TEMP_NOTIFICATION_ENABLED,
    BLE_TEMP_NOTIFICATION_DISABLED,
    BLE_CUS_EVT_COMMAND_RX,  

} ble_cus_evt_type_t;

/*
 * @details This structure is passed to an event when @ref BLE_CUS_EVT_COMMAND_RX occurs.
 */
typedef struct 
{
    uint8_t const * p_data;   
    uint16_t   length;  
} command_data_t;


/**@brief Custom Service event. */
typedef struct
{
   ble_cus_evt_type_t evt_type;                     

   union
   {
       command_data_t command_data;
       
   } params_command;

} ble_cus_evt_t;

// Forward declaration of the ble_cus_t type.
typedef struct ble_cus_s ble_cus_t;


/**@brief Csutom service event handler type. */
typedef void (*ble_cus_evt_handler_t) (ble_cus_t * p_cus, ble_cus_evt_t * p_evt);


/**@brief Custom Service init structure. This contains all options and data needed for
 *        initialization of the service.*/
typedef struct
{
    ble_cus_evt_handler_t         evt_handler;                        /**< Event handler to be called for handling events in the Custom Service. */  
   
    ble_srv_cccd_security_mode_t  temperature_char_attr_md;      /**< Initial security level for the temperature characteristic attribute */
    ble_srv_cccd_security_mode_t  command_char_attr_md;          /**< Initial security level for the command characteristic attribute */

} ble_cus_init_t;


/**@brief Custom Service structure. This contains various status information for the service. */
struct ble_cus_s
{
    ble_cus_evt_handler_t         evt_handler;                    /**< Event handler to be called for handling events in the Custom Service. */
    uint16_t                      service_handle;                 /**< Handle of Custom Service (as provided by the BLE stack). */
   
    ble_gatts_char_handles_t      temperature_value_handles;      /**< Handles related to the temperature characteristic. */
    ble_gatts_char_handles_t      command_value_handles;          /**< Handles related to the command characteristic. */
     
    uint16_t                      conn_handle;                    /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection). */
    uint8_t                       uuid_type; 
};


/**@brief Function for handling the Custom servie ble events.
 *
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
void ble_cus_on_ble_evt( ble_evt_t const * p_ble_evt, void * p_context);


/**@brief Function for initializing the Custom ble service.
 *
 * @param[in]   p_cus       Custom service structure.
 * @param[in]   p_cus_init  Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */

uint32_t ble_cus_init(ble_cus_t * p_cus, const ble_cus_init_t * p_cus_init);


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

uint32_t temperature_value_update(ble_cus_t * p_cus, uint8_t  * p_data, uint16_t  p_length);
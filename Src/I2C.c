
#include "I2C.h"

#define NRF_LOG_MODULE_NAME "I2C"
//#include <legacy/nrf_drv_twi.h>

// I2C instance.
#define TWI_INSTANCE_ID 0
#define APP_IRQ_PRIORITY_LOW 3  //overrides definition elsewhere
#define BA_SDA_PIN      26       // 5 SDA signal pin
#define BA_SCL_PIN      27       // 8 SCL signal pin
#define BA_VDD_PIN      2

static const nrf_drv_twi_t i2c = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);

// Indicates if operation on TWI has ended.
static volatile bool m_xfer_done = false;

// @brief UART initialization.
void I2C_init(void)
{
	//NRF_LOG_DEBUG("twi_init(void)\r\n");
	// Set Vdd for BMA280 on port P0.02 (For example P1.07: nrf_gpio_pin_write(39, 1) where 32+7 = 39.)
  nrf_gpio_cfg_output(BA_VDD_PIN);
	nrf_gpio_pin_set(BA_VDD_PIN);
	
	ret_code_t err_code;

	const nrf_drv_twi_config_t i2c_config = 
	{
		.scl = BA_SCL_PIN,
		.sda = BA_SDA_PIN,
		.frequency = NRF_TWI_FREQ_100K,
		.interrupt_priority = APP_IRQ_PRIORITY_LOW,
		.clear_bus_init = false
	};

	//last one is some kind of context - no idea what that is....
	//documentation is vague/nonexistant
	err_code = nrf_drv_twi_init(&i2c, &i2c_config, I2C_handler, NULL);
	APP_ERROR_CHECK(err_code);

	nrf_drv_twi_enable(&i2c); 
    
		
	//NRF_LOG_DEBUG("I2C_init(void) done\r\n");
}

void writeByte(uint8_t address, uint8_t subAddress, uint8_t data)
{
	uint8_t temp[2];
    
	temp[0] = subAddress;
	temp[1] = data;
    
	ret_code_t err_code;
	m_xfer_done = false;
	err_code = nrf_drv_twi_tx(&i2c, address, &temp[0], 2, true);
	APP_ERROR_CHECK(err_code);
	while (m_xfer_done == false) ; //wait until end of transfer
}

void writeBytes(uint8_t address, uint8_t * data, uint8_t n_bytes)
{
	ret_code_t err_code;    
	m_xfer_done = false;
	err_code = nrf_drv_twi_tx(&i2c, address, data, n_bytes, false); //false = close the channel - not waiting for response 
	APP_ERROR_CHECK(err_code);
	while (m_xfer_done == false) ; //wait until end of transfer -- seriously??
}
 
uint8_t readByte(uint8_t address, uint8_t subAddress)
{
	ret_code_t err_code = 0;
    
	uint8_t value;
    
	m_xfer_done = false;
	//NRF_LOG_DEBUG("readByte - Writing\r\n");
	//last position is the transfer pending flag
	err_code = nrf_drv_twi_tx(&i2c, address, &subAddress, 1, true);
    
	APP_ERROR_CHECK(err_code);
    
	while (m_xfer_done == false) ; //wait until end of transfer
    
	if(err_code == NRF_SUCCESS)
	{
		m_xfer_done = false;
		//NRF_LOG_DEBUG("readByte - Reading\r\n");
		err_code = nrf_drv_twi_rx(&i2c, address, &value, 1);
		APP_ERROR_CHECK(err_code);
        
		while (m_xfer_done == false) ;
	}
	;
    
	//NRF_LOG_DEBUG("readByte done, returned 0x\r\n");
	//NRF_LOG_HEXDUMP_DEBUG(&value, 1);
	//NRF_LOG_FLUSH();
	return value;
}

// @brief TWI events handler.
void I2C_handler(nrf_drv_twi_evt_t const * p_event, void * p_context)
{
	switch (p_event->type)
	{
	case NRF_DRV_TWI_EVT_DONE:
            
		//todo -difference between read and write???
		m_xfer_done = true;
            
		if (p_event->xfer_desc.type == NRF_DRV_TWI_XFER_RX)
		{
			//SEGGER_RTT_WriteString(0, "Data just came back!\n");  
		}
            
		//NRF_LOG_DEBUG("I2C_handler responding to NRF_DRV_TWI_EVT_DONE\r\n");
		break;
	default:
		break;
	}
}

//readBytes(BME280_ADDRESS_1, BME280_PRESS_MSB, 8, &rawData[0]);  

void readBytes(uint8_t address, uint8_t subAddress, uint8_t * dest, uint8_t n_bytes)
{
	ret_code_t err_code = 0;
	//0xF7 to 0xFE (temperature, pressure and humidity)
	//readBytes(BME280_ADDRESS_1, BME280_PRESS_MSB, 9, &rawData[0]);  
    
	m_xfer_done = false;
    
	err_code = nrf_drv_twi_tx(&i2c, address, &subAddress, 1, true);
    
	while (m_xfer_done == false) {}
	;
    
	//comes back with error code
	//SEGGER_RTT_printf(0, "ReadBytes code: %d\n", err_code);
	//here is the problem????
	//APP_ERROR_CHECK(err_code);
    
	if(err_code == NRF_SUCCESS)
	{
		m_xfer_done = false;
		//NRF_LOG_DEBUG("readBytes - Reading\r\n");
		err_code = nrf_drv_twi_rx(&i2c, address, dest, n_bytes);
		//SEGGER_RTT_printf(0, "ReadBytes RX code: %d\n", err_code);
		//APP_ERROR_CHECK(err_code);
		while(m_xfer_done == false) {}
		;
	}
	;

	//NRF_LOG_DEBUG("readBytes done\r\n");
	//NRF_LOG_FLUSH();
}

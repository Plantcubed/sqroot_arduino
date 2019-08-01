/** 
 *  \file sensor_gc0022.h
 *  \brief Sensor module for air co2, temperature, and humidity.
 */

// Library based off: Cozir Example Sketch from CO2Meter.com
// Library found at: http://www.co2meters.com/Documentation/AppNotes/AN128-%20Cozir_Arduino.pdf
// Component found at: http://www.co2meter.com/products/cozir-0-2-co2-sensor
// Modified by Jake Rye
// July 15, 2015

#ifndef PLANT3_SENSOR_GC0022_H
#define PLANT3_SENSOR_GC0022_H

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include "support_software_serial.h"
#include "module_handler.h"

#define	DATA_MSG		0
#define STATUS_MSG		1

/** 
 *  \brief Sensor module for air co2, temperature, and humidity.
 */
class Plant3_SensorGc0020 : SensorActuatorModule {
  public:
    // Public Functions
	Plant3_SensorGc0020(int rx_pin, int tx_pin, String co2_instruction_code, int co2_instruction_id, String temperature_instruction_code, int temperature_instruction_id,String humidity_instruction_code, int humidity_instruction_id);
    void begin(void);
    String get(int type);
    String set(String instruction_code, int instruction_id, String instruction_parameter);

    // Public Variables
    float temperature;
    float humidity;
    float co2;
   
  private:
    // Private Functions
    void getSensorData(void);
    void sendMessage(String message);
    String receiveMessage();

    // Private Variables
    int rx_pin_;
    int tx_pin_;
    String co2_instruction_code_;
    int co2_instruction_id_;     
    String temperature_instruction_code_;
    int temperature_instruction_id_;   
    String humidity_instruction_code_;
    int humidity_instruction_id_; 
    SoftwareSerial *ss_;
    uint32_t timeout_;

	uint32_t status_;
};

#endif // PLANT3_SENSOR_GC0022_H

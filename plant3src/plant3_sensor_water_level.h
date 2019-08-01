/** 
 *  \file sensor_dfr0161_0300.h
 *  \brief Sensor module for water ph, ec, and temperature. 
 *  \details These 3 sensors are contained in the same module due to their dependencies.
 *  Water temperature is used to calculate ec and when the ec sensor is on, it 
 *  interferes with the ph sensor. Temperature is updated every time the the *.get()
 *  function is called. Ph and ec will alternate updates assuming the time since the 
 *  previous *.get() was called is greater than the required delay durations for the 
 *  amount of time the ec sensor needs to turn on as well as the the amount of time
 *  the ph sensor needs for ec_sensor noise to settle once turned off. As implied by
 *  the previous statement, the ec sensor's GND and VCC lines are tied to a DPST-NO 
 *  relay (2-TypeA) that is controlled by the ec_enable_pin. As some relays can be very 
 *  loud when switching, a reed relay should be used instead of a latching relay. The audible 
 *  noise produced from the reed relay used in this system is neglidgable. As 
 *  audible noise can be bothersome to have in a system, a reasonable thought might be 
 *  to use a solid-state relay. This will not work as solid state relays do not provide 
 *  complete electrical isolation which will cause the ec sensor to interfere with the ph 
 *  sensor thus defeating the purpose of having the relay. If alternating ph and ec sensor
 *  reading is inadequate to your sensing purposes, analog optoisolators may be of use. The
 *  specific components intended for use with this module are the DFR0161 pH sensor, DFR0300
 *  EC sensor, and the DS18B20 temperature sensor. The temperature sensor is included in the 
 *  DFR0300 package. The relay used is the 8L02-05-01.
 *  \author Jake Rye
 */
#ifndef PLANT3_SENSOR_WATER_LEVEL_H
#define PLANT3_SENSOR_WATER_LEVEL_H


#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include "module_handler.h"
#include "support_moving_average.h"
#include "support_one_wire.h"

#define DEFAULT_SAMPLES		3
#define DEFAULT_NORMAL		0

/**
 * \brief Sensor module for water ph, ec, and temperature.
 */
class Plant3SensorWaterLevel : SensorActuatorModule {
  public:
    // Public Functions
    /*
     * \brief Class constructor.
     */
	  Plant3SensorWaterLevel(int level_pin, String level_instruction_code, int level_instruction_id, int cir_pin, String cir_instruction_code, int cir_id);

    /**
     * \brief Called once to setup module.
     * Declares objects, configures initial state, sets configuration & calibration parameters.
     */
    void begin();
    
    /**
     * \brief Returns JSON string with module data.
     * Module data: ph, temperature, ec.
     * Data: "<instruction_code> <instruction_id> <value>".
     * Example: "SWPH 1 1", "SWTM 1 1", "SWEC 1 1", 
     */
    String get(int type);

    /**
     * \brief Reserved to passing data string to object
     * Currently unused. Exists to comply with SAModule interface.
     */
    String set(String instruction_code, int instruction_id, String instruction_parameter);

   
  private:

    // Private Functions
    int getSensorData(void);
	int getSensorDataRaw(void);
	void turnOn(void);
	void turnOff(void);

    // Private Variables
	int sensor_array[DEFAULT_SAMPLES];
	int level_;
	int EE_Address_;
    int level_pin_;
	int cir_value_;
    String level_instruction_code_;
    int level_instruction_id_;
    int cir_pin_;
    String cir_instruction_code_;
	int cir_instruction_id_;
    int cir_id_;
	uint32_t status_;
	bool safety_turnoff;
	int filter_index;
};

#endif // SENSOR_DFR0161_0300_H_



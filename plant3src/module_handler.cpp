/** 
 *  \file module_handler.cpp
 *  \brief Handles all module integration. 
 *  \details See module_handler.h for details.
 *  \author Jake Rye
 */
#include "module_handler.h"

// Include Module Libraries
#include "communication.h"
#include "sensor_tsl2561.h"
#include "plant3_sensor_gc0020.h"
#include "plant3_actuator_mostfet.h"
#include "plant3_actuator_mosfet_pwm.h"
#include "plant3_actuator_heater.h"
#include "plant3_lighting.h"
#include "plant3_actuator_A3909Ghbridge.h"
#include "plant3_actuator_ph.h"
#include "plant3_sensor_phectemp.h"
#include "plant3_sensor_water_level.h"

#define REV				"\"GVER\":\"0.6.271\","
int EE_Address;
int datacounter;

// Declare Module Objects
Communication communication;
Plant3SensorWaterLevel sensor_growtank_water_level_default(A3, "SWLV", 1, 33, "AWCR", 1);
SensorTsl2561 sensor_tsl2561_light_intensity_default("SLIN", 1, "SLPA", 1);
Plant3SensorPhECTemp sensor_water_ph_temperature_ec_default(A1, "SWPH", 1, 5, "SWTM", 1, A2, 7, "SWEC", 1);
Plant3_SensorGc0020 sensor_gc0020_air_co2_default(12, 11, "SACO", 1, "SATM", 1, "SAHU", 1);
Plant3_ActuatorHeater actuator_relay_air_heater_default("AAHE", 1, 6, 2);
Plant3_Lighting chamber_illumination_default("ALPN", 2, 4, "ALPN", 1, 8, "SGWO", 1, 3);
Plant3_ActuatorMosFET actuator_relay_air_humidifier_default(9, "AAHU", 1); 
Plant3_ActuatorMosFET actuator_relay_air_vent_default(14, "AAVE", 1);
Plant3_ActuatorMosFET actuator_relay_air_circulation_default(34, "AACR", 1);
Plant3_ActuatorA3909GHBridge dose_pump1_default("AWLV", 1, 38, 39, 1, 0); // dose 1
Plant3_ActuatorPH ph_pump_default("AWPH", 1, 40, 41, 42, 43, 1, 0); // dose 2 & dose 3
Plant3_ActuatorA3909GHBridge dose_pump4_default("AWEC", 1, 44, 45, 1, 0); // dose 4
Plant3_ActuatorMosFET actuator_relay_co2_default(46, "AACO", 1);
Plant3_ActuatorMosFET_PWM actuator_relay_waterair_default(10, "AWAR", 1, 12, 1.5);
Plant3_ActuatorMosFET actuator_relay_coolvalve_default(19, "AACL", 1);

void initializeModules(void) {
	datacounter = 8;  // a few data messsages then a status
	EE_Address = 0;
	communication.begin();
	sensor_water_ph_temperature_ec_default.begin();
	sensor_tsl2561_light_intensity_default.begin();
	sensor_gc0020_air_co2_default.begin();
	actuator_relay_air_heater_default.begin();
	chamber_illumination_default.begin();
	actuator_relay_air_humidifier_default.begin();
	actuator_relay_air_vent_default.begin();
	actuator_relay_air_circulation_default.begin();
	dose_pump1_default.begin();
	ph_pump_default.begin();
	actuator_relay_co2_default.begin();
	sensor_growtank_water_level_default.begin();
	actuator_relay_waterair_default.begin();
	actuator_relay_coolvalve_default.begin();

	// Set Default States
	actuator_relay_air_heater_default.set("AAHE", 1, "0");
	actuator_relay_air_circulation_default.set("AACR", 1, "0");
	actuator_relay_air_vent_default.set("AAVE", 1, "0");
	chamber_illumination_default.set("ALPN", 1, "0");
	chamber_illumination_default.set("ALPN", 2, "0");
	dose_pump1_default.set("AWLV", 1, "0");
	ph_pump_default.set("AWPH", 1, "0");
	dose_pump4_default.set("AWEC", 1, "0");
	actuator_relay_co2_default.set("AACO", 1, "0");
	sensor_growtank_water_level_default.set("AWCR", 1, "0");
	actuator_relay_waterair_default.set("AWAR", 1, "0");
	actuator_relay_coolvalve_default.set("AACL", 1, "0");
	actuator_relay_air_humidifier_default.set("AAHU", 1, "0");
}

void updateIncomingMessage(void) {
  // Check for Message(s) And Handle If Necessary
  String response_message = "";
  while (communication.available()) { // read in message(s) until nothing in serial buffer
    response_message += handleIncomingMessage();
  }
  // Append Responses From Message(s) Then Send
  if (response_message != "") {
    response_message = "\"GTYP\":\"Response\"," + response_message;
    response_message += "\"GEND\":0";
    communication.send(response_message);
  }
}

void updateStreamMessage(void) {
	String stream_message;
	//if (datacounter < 10 ) {
		stream_message = GetStreamMessage(DATA_MSG);
	/*	datacounter++;
	}
	else if (datacounter == 10) {
		stream_message = GetStreamMessage(STATUS_MSG);
		datacounter = 0;
	}*/
	communication.send(stream_message);
}

String GetStreamMessage(int type) {
	String stream_message;

	if ( type == DATA_MSG)
		stream_message = "\"GTYP\":\"Stream\",";
	else stream_message = "\"GTYP\":\"Status\",";

	// add version information
	stream_message += REV;
	// Get Stream Message
	stream_message += sensor_growtank_water_level_default.get(type);
	stream_message += sensor_water_ph_temperature_ec_default.get(type);
	stream_message += sensor_tsl2561_light_intensity_default.get(type);
	stream_message += sensor_gc0020_air_co2_default.get(type);
	stream_message += chamber_illumination_default.get(type);
	stream_message += actuator_relay_air_heater_default.get(type);
	stream_message += actuator_relay_air_humidifier_default.get(type);
	stream_message += actuator_relay_air_vent_default.get(type);
	stream_message += actuator_relay_air_circulation_default.get(type);
	stream_message += actuator_relay_co2_default.get(type);
	stream_message += actuator_relay_waterair_default.get(type);
	stream_message += actuator_relay_coolvalve_default.get(type);
	stream_message += dose_pump1_default.get(type);
	stream_message += ph_pump_default.get(type);
	stream_message += dose_pump4_default.get(type);
	stream_message += "\"GEND\":0";

	// Return Stream Message
	return(stream_message);
}


String handleIncomingMessage(void) {
  // Parse Message into: Instruction Code - ID - Parameter
  String return_message = "";
  String incoming_message = communication.receive();
  Instruction instruction = parseIncomingMessage(incoming_message);
  // Pass Parsed Message To All Objects and Update Return Message if Applicable
  if (instruction.valid) {
    return_message += actuator_relay_air_heater_default.set(instruction.code, instruction.id, instruction.parameter);
    return_message += actuator_relay_air_humidifier_default.set(instruction.code, instruction.id, instruction.parameter);
    return_message += actuator_relay_air_vent_default.set(instruction.code, instruction.id, instruction.parameter);
    return_message += actuator_relay_air_circulation_default.set(instruction.code, instruction.id, instruction.parameter);
	return_message += actuator_relay_co2_default.set(instruction.code, instruction.id, instruction.parameter);
	return_message += actuator_relay_waterair_default.set(instruction.code, instruction.id, instruction.parameter);
	return_message += actuator_relay_coolvalve_default.set(instruction.code, instruction.id, instruction.parameter);
	return_message += dose_pump1_default.set(instruction.code, instruction.id, instruction.parameter);
	return_message += ph_pump_default.set(instruction.code, instruction.id, instruction.parameter);
	return_message += dose_pump4_default.set(instruction.code, instruction.id, instruction.parameter);
	return_message += chamber_illumination_default.set(instruction.code, instruction.id, instruction.parameter);
	return_message += sensor_growtank_water_level_default.set(instruction.code, instruction.id, instruction.parameter);
	return_message += sensor_water_ph_temperature_ec_default.set(instruction.code, instruction.id, instruction.parameter);
	return_message += sensor_tsl2561_light_intensity_default.set(instruction.code, instruction.id, instruction.parameter);
	return_message += sensor_gc0020_air_co2_default.set(instruction.code, instruction.id, instruction.parameter);
  }

  return return_message;
}

Instruction parseIncomingMessage(String message) {
  // Initialize Instruction
  Instruction instruction;
  instruction.valid = 0;

  // Get Instruction Data
  int len = message.length();
  int first_space = message.indexOf(" ");
  if ((first_space > 0) && (len > first_space)) {
    int second_space = message.indexOf(" ", first_space + 1);
    if ((second_space > 0) && (second_space < len - 1)) {
      // Received good message
      instruction.code = message.substring(0, 4);
      instruction.id = (message.substring(first_space, second_space)).toInt();
      instruction.parameter = message.substring(second_space + 1, len);
      instruction.valid = 1;
    }
  }

  // Return Instruction Data
  return instruction;
}

int Get_EE_AddressPtr(void)
{
	return EE_Address;
}

void Set_EE_AddressPtr(int NewAddress)
{
	EE_Address = NewAddress;
}


/** 
 *  \file plant3_sensor_water_level.cpp
 *  \brief Sensor module for Tape Level Sensor
 *  \details See plant3_sensor_water_level.h for details.
 *  \author Richard Klosinski
 */
#include "plant3_sensor_water_level.h"
#include "error.h"

//------------------------------------------PUBLIC FUNCTIONS----------------------------------------//
Plant3SensorWaterLevel::Plant3SensorWaterLevel(int level_pin, String level_instruction_code, int level_instruction_id, int cir_pin, String cir_instruction_code, int cir_id) {
  level_pin_ = level_pin;
  level_instruction_code_ = level_instruction_code;
  level_instruction_id_ = level_instruction_id;
  cir_pin_ = cir_pin;
  cir_instruction_code_ = cir_instruction_code;
  cir_instruction_id_ = cir_id;
  filter_index = 0;
}

void Plant3SensorWaterLevel::begin() {

  // Configure Initial State
  pinMode(cir_pin_, OUTPUT);
  pinMode(level_pin_, INPUT);
  level_ = 1;
  // fill the sample filter que
  for (int a = 0; a < DEFAULT_SAMPLES * 2; a++) {
	  level_ = getSensorData();
  }
  turnOff();
  status_ = ERROR_NONE;
}

String Plant3SensorWaterLevel::get(int type) {
  // Get water level
  
  level_ = getSensorData();

  // turn off circulation pump if too low
 // if (level_ == 0) {
	//  // water low. turn off cir pump
	//  safety_turnoff = true;
	//  status_ |= CIR_PUMP_OFF;
	//  digitalWrite(cir_pin_, LOW);
 // }
 // else {
	//if (cir_value_ == 1) turnOn();
	//else turnOff();
	//status_ &= ~CIR_PUMP_OFF;
 // }

  // Initialize Message
  String message = "";

  // Append Level
  message += "\"";
  message += level_instruction_code_;
  message += " ";
  message += level_instruction_id_;
  message += "\":";
  if (type == DATA_MSG) message += level_;
  else message += status_;
  message += ",";

  // Append Circulation Pump 
  message += "\"";
  message += cir_instruction_code_;
  message += " ";
  message += cir_instruction_id_;
  message += "\":";
  if (type == DATA_MSG) message += cir_value_;
  else message += status_;
  message += ",";

  // Return Message
  return message;
}

String Plant3SensorWaterLevel::set(String instruction_code, int instruction_id, String instruction_parameter)
{
	if ((instruction_code == cir_instruction_code_) && (instruction_id == cir_instruction_id_)) {
		if (instruction_parameter.toInt() == 1) {
			turnOn();
			return "";
		}
		else if (instruction_parameter.toInt() == 0) {
			turnOff();
			return "";
		}
	}
	return "";
}

//------------------------------------------PRIVATE FUNCTIONS----------------------------------------//

void Plant3SensorWaterLevel::turnOn(void) {
	//if (level_ == 0) {
	//	status_ |= CIR_PUMP_OFF;
	//	safety_turnoff = true;
	//}
	//else {
	digitalWrite(cir_pin_, HIGH); // active low relay
	status_ &= ~CIR_PUMP_OFF;
	safety_turnoff = false;
	//}
	cir_value_ = 1;
}

void Plant3SensorWaterLevel::turnOff(void) {
	digitalWrite(cir_pin_, LOW);
	cir_value_ = 0;
}

int Plant3SensorWaterLevel::getSensorData(void) {
	int sum = 0;
	sensor_array[filter_index] = getSensorDataRaw();
	filter_index++;
	if (filter_index >= DEFAULT_SAMPLES) filter_index = 0;
	for (int a = 0; a < DEFAULT_SAMPLES; a++) sum += sensor_array[a];
	if (sum == DEFAULT_SAMPLES) return (1);
	else if (sum == 0) return (0);
	
	return (level_);
}

int Plant3SensorWaterLevel::getSensorDataRaw(void) {
	return digitalRead(level_pin_);
}





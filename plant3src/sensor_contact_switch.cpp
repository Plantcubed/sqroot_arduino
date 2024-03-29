/** 
 *  \file sensor_contact_switch.cpp
 *  \brief Sensor module for all sensors that behave like a contact switch.
 *  \details See sensor_contact_switch.h for details.
 *  \author Jake Rye
 */
#include "sensor_contact_switch.h"
#include "error.h"

//--------------------------------------------------PUBLIC-------------------------------------------//
SensorContactSwitch::SensorContactSwitch(int pin, String instruction_code, int instruction_id) {
 pin_ = pin;
 instruction_code_ = instruction_code;
 instruction_id_ = instruction_id;
}

void SensorContactSwitch::begin(void) {
 pinMode(pin_,INPUT_PULLUP);
 status_ = ERROR_NONE;
}

String SensorContactSwitch::get(int type) {
  // Get Sensor Data
  is_connected_ = getData();

  // Initialize Message
  String message = "";

  // Append Actuator State
  message += "\"";
  message += instruction_code_;
  message += " ";
  message += instruction_id_;
  message += "\":";
  if (type == DATA_MSG) message += is_connected_;
  else message += status_;
  message += ",";

  // return "";
  return message;
}

String SensorContactSwitch::set(String instruction_code, int instruction_id, String instruction_parameter) {
  return "";
}

//-------------------------------------------------PRIVATE-------------------------------------------//
bool SensorContactSwitch::getData(void) {
  return digitalRead(pin_);
}


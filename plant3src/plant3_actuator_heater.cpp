/** 
 *  \file actuator_relay.cpp
 *  \brief Actuator module for an active low SPST-NO relay.
 *  \details See plant3_actuator_heater.h for details.
 *  \author Richard Klosinski
 */
#include "plant3_actuator_heater.h"
#include "error.h"

//--------------------------------------------------PUBLIC-------------------------------------------//
Plant3_ActuatorHeater::Plant3_ActuatorHeater(String instruction_code, int instruction_id, int heater_pin, int fan_pin) {
 heater_pin_ = heater_pin;
 fan_pin_ = fan_pin;
 instruction_code_ = instruction_code;
 instruction_id_ = instruction_id;
}

void Plant3_ActuatorHeater::begin(void) {
 pinMode(heater_pin_,OUTPUT);
 pinMode(fan_pin_, OUTPUT);
 turnOff();
 status_ = ERROR_NONE;
}

String Plant3_ActuatorHeater::get(int type) {
  // Initialize Message
  String message = "";

  // Append Actuator State
  message += "\"";
  message += instruction_code_;
  message += " ";
  message += instruction_id_;
  message += "\":";
  if (type == DATA_MSG) message += value_;
  else message += status_;
  message += ",";
   
  return message;
}

String Plant3_ActuatorHeater::set(String instruction_code, int instruction_id, String instruction_parameter) {
  if ((instruction_code == instruction_code_) && (instruction_id == instruction_id_)) {
    if (instruction_parameter.toInt() == 1) {
      turnOn();
      return "";
    }
    else if(instruction_parameter.toInt() == 0) {
      turnOff();
      return "";
    }
  }
  return "";
}

//-------------------------------------------------PRIVATE-------------------------------------------//
void Plant3_ActuatorHeater::turnOn(void){
	digitalWrite(heater_pin_, HIGH); 
	digitalWrite(fan_pin_, HIGH);
	value_ = 1;
}

void Plant3_ActuatorHeater::turnOff(void){
	digitalWrite(heater_pin_, LOW);
	digitalWrite(fan_pin_, LOW);
	value_ = 0;
}

/**
*  \file actuator_relay.cpp
*  \brief Actuator module for an active low SPST-NO relay.
*  \details See actuator_relay.h for details.
*  \author Jake Rye
*/
#include "plant3_actuator_mosfet_pwm.h"
#include "error.h"

//--------------------------------------------------PUBLIC-------------------------------------------//
Plant3_ActuatorMosFET_PWM::Plant3_ActuatorMosFET_PWM(int pin, String instruction_code, int instruction_id, int applied, int target) {
	pin_ = pin;
	instruction_code_ = instruction_code;
	instruction_id_ = instruction_id;
	applied_ = applied;
	target_ = target;
}

void Plant3_ActuatorMosFET_PWM::begin(void) {
	pinMode(pin_, OUTPUT);
	turnOff();
	status_ = ERROR_NONE;
}

String Plant3_ActuatorMosFET_PWM::get(int type) {
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

String Plant3_ActuatorMosFET_PWM::set(String instruction_code, int instruction_id, String instruction_parameter) {
	if ((instruction_code == instruction_code_) && (instruction_id == instruction_id_)) {
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

//-------------------------------------------------PRIVATE-------------------------------------------//
void Plant3_ActuatorMosFET_PWM::turnOn(void) {
	int temp = 255 / (applied_ / target_);
	analogWrite(pin_, temp); // active low relay
	value_ = 1;
}

void Plant3_ActuatorMosFET_PWM::turnOff(void) {
	analogWrite(pin_, 0);
	value_ = 0;
}

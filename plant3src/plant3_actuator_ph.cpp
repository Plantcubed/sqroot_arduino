/**
*  \file actuator_hbridge.cpp
*  \brief Actuator module for hbridge driver.
*  \details See actuator_hbrige.h for details.
*  \author Richard Klosinski
*/
#include "plant3_actuator_ph.h"
#include "error.h"

#define  MS_SECOND						1000
#define	 TIMEOUT_TEN_SECONDS			(10 * MS_SECOND)
#define	 TIMEOUT_THIRTY_SECONDS			(30 * MS_SECOND)

//--------------------------------------------------PUBLIC-------------------------------------------//
Plant3_ActuatorPH::Plant3_ActuatorPH(String instruction_code, int instruction_id, int up_pinA, int up_pinB, int down_pinA, int down_pinB, int forwarddirection, unsigned long timeout) {
	up_pinA_ = up_pinA;
	up_pinB_ = up_pinB;
	down_pinA_ = down_pinA;
	down_pinB_ = down_pinB;
	instruction_code_ = instruction_code;
	instruction_id_ = instruction_id;
	if (timeout == 0) timeout_ = TIMEOUT_TEN_SECONDS;
	else timeout_ = timeout;
}

void  Plant3_ActuatorPH::begin(void) {
	pinMode(up_pinA_, OUTPUT);
	pinMode(up_pinB_, OUTPUT);
	pinMode(down_pinA_, OUTPUT);
	pinMode(down_pinB_, OUTPUT);
	StopAll();
	last_read_time_ = millis();
	status_ = ERROR_NONE;
	value_ = 0;
}

String  Plant3_ActuatorPH::get(int type) {
	unsigned long current_time;
	// Initialize Message
	String message = "";

	current_time = millis();
	if (current_time < last_read_time_) {
		// ie there was a rollover
		last_read_time_ = 0;
	}
	if ((current_time - last_read_time_) > timeout_) {
		// turn off pumps
		StopAll();
	}
	// Append Actuator State
	message += "\"";
	message += instruction_code_;
	message += " ";
	message += instruction_id_;
	message += "\":";
	if (type == DATA_MSG) message += value_;
	else  message += status_;
	message += ",";

	return message;
}

String Plant3_ActuatorPH::set(String instruction_code, int instruction_id, String instruction_parameter) {
	if ((instruction_code == instruction_code_) && (instruction_id == instruction_id_)) {
		if (instruction_parameter.toInt() == 1) {
			StopDown();
			UpForward();
			last_read_time_ = millis();
			return "";
		}
		else if (instruction_parameter.toInt() == -1) {
			StopUp();
			DownForward();
			last_read_time_ = millis();
			return "";
		}
		else if (instruction_parameter.toInt() == 0) {
			StopAll();
			last_read_time_ = millis();
			return "";
		}
	}
	return "";
}

//-------------------------------------------------PRIVATE-------------------------------------------//
void  Plant3_ActuatorPH::UpForward(void) {
	digitalWrite(up_pinA_, LOW);
	digitalWrite(up_pinB_, HIGH);
	value_ = 1;
}

void  Plant3_ActuatorPH::UpReverse(void) {
	digitalWrite(up_pinA_, HIGH);
	digitalWrite(up_pinB_, LOW);
	value_ = -1;
}

void  Plant3_ActuatorPH::StopUp(void) {
	digitalWrite(up_pinA_, LOW);
	digitalWrite(up_pinB_, LOW);
	value_ = 0;
}

void  Plant3_ActuatorPH::DownForward(void) {
	digitalWrite(down_pinA_, LOW);
	digitalWrite(down_pinB_, HIGH);
	value_ = 1;
}

void  Plant3_ActuatorPH::DownReverse(void) {
	digitalWrite(down_pinA_, HIGH);
	digitalWrite(down_pinB_, LOW);
	value_ = -1;
}

void  Plant3_ActuatorPH::StopDown(void) {
	digitalWrite(down_pinA_, LOW);
	digitalWrite(down_pinB_, LOW);
	value_ = 0;
}

void  Plant3_ActuatorPH::StopAll(void) {
	digitalWrite(up_pinA_, LOW);
	digitalWrite(up_pinB_, LOW);
	digitalWrite(down_pinA_, LOW);
	digitalWrite(down_pinB_, LOW);
	value_ = 0;
}


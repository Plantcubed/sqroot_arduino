/** 
 *  \file actuator_hbridge.cpp
 *  \brief Actuator module for hbridge driver.
 *  \details See actuator_hbrige.h for details.
 *  \author Richard Klosinski
 */
#include "plant3_actuator_A3909Ghbridge.h"
#include "error.h"

#define  MS_SECOND						1000
#define	 TIMEOUT_TEN_SECONDS			(10 * MS_SECOND)
#define	 TIMEOUT_THIRTY_SECONDS			(30 * MS_SECOND)

//--------------------------------------------------PUBLIC-------------------------------------------//
Plant3_ActuatorA3909GHBridge::Plant3_ActuatorA3909GHBridge(String instruction_code, int instruction_id, int pinA, int pinB, int forwarddirection, unsigned long timeout) {
 pinA_ = pinA;
 pinB_ = pinB;
 forwarddirection_ = forwarddirection;
 instruction_code_ = instruction_code;
 instruction_id_ = instruction_id;
 timeout_ = timeout;
}

void  Plant3_ActuatorA3909GHBridge::begin(void) {
 pinMode(pinA_,OUTPUT);
 pinMode(pinB_, OUTPUT);
 Stop();
 last_read_time_ = millis();
 status_ = ERROR_NONE;
}

String  Plant3_ActuatorA3909GHBridge::get(int type) {
  unsigned long current_time;
  // Initialize Message
  String message = "";

  if (timeout_ > 0) {
	  current_time = millis();
	  if (current_time < last_read_time_) {
		  // ie there was a rollover
		  last_read_time_ = 0;
	  }
	  if ((current_time - last_read_time_) > timeout_) {
		  // turn off pumps
		  Stop();
	  }
  }
  // Append Actuator State
  message += "\"";
  message += instruction_code_;
  message += " ";
  message += instruction_id_;
  message += "\":";
  if ( type == DATA_MSG ) message += value_;
  else  message += status_;
  message += ","; 
  
  return message;
}

String  Plant3_ActuatorA3909GHBridge::set(String instruction_code, int instruction_id, String instruction_parameter) {
	String temp;

	String smode = getValue(instruction_parameter, ' ', 0);
	int mode = smode.toInt();

	if ((instruction_code == instruction_code_) && (instruction_id == instruction_id_)) {
		switch (mode) {
		case 1:
			Forward();
			last_read_time_ = millis();
			break;
		case 0:
			Stop();
			last_read_time_ = millis();
			break;
		case -1:
			Reverse();
			last_read_time_ = millis();
		case 2:
			temp = getValue(instruction_parameter, ' ', 1);
			timeout_ = temp.toInt();
			break;
		}
  }
  return "";
}

//-------------------------------------------------PRIVATE-------------------------------------------//
void  Plant3_ActuatorA3909GHBridge::Forward(void){
	if (forwarddirection_ == 1) {
		digitalWrite(pinA_, LOW); 
		digitalWrite(pinB_, HIGH); 
	}
	else {
		digitalWrite(pinA_, HIGH);
		digitalWrite(pinB_, LOW);
	}
	value_ = 1;
}

void  Plant3_ActuatorA3909GHBridge::Reverse(void) {
	if (forwarddirection_ == 1) {
		digitalWrite(pinA_, HIGH);
		digitalWrite(pinB_, LOW);
	}
	else {
		digitalWrite(pinA_, LOW);
		digitalWrite(pinB_, HIGH);
	}
	value_ = -1;
}

void  Plant3_ActuatorA3909GHBridge::Stop(void){
  digitalWrite(pinA_, LOW);
  digitalWrite(pinB_, LOW);
  value_ = 0;
}

String Plant3_ActuatorA3909GHBridge::getValue(String data, char separator, int index)
{
	int found = 0;
	int strIndex[] = {
		0, -1 };
	int maxIndex = data.length() - 1;
	for (int i = 0; i <= maxIndex && found <= index; i++) {
		if (data.charAt(i) == separator || i == maxIndex) {
			found++;
			strIndex[0] = strIndex[1] + 1;
			strIndex[1] = (i == maxIndex) ? i + 1 : i;
		}
	}
	return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

/**
*  \file illum_panel.cpp
*  \brief Illumination Planel Control
*  \details See illum_panel.h for details.
*  \author Richard Klosinski
*/
#include "plant3_lighting.h"
#include "error.h"

//----------------------------------------------PUBLIC------------------=----------------------------//
Plant3_Lighting::Plant3_Lighting(String white_instruction_code, int white_instruction_id, int white_pin, String grow_instruction_code, int grow_instruction_id, int grow_pin, String door_instruction_code, int door_instruction_id, int door_pin) {
	white_instruction_code_ = white_instruction_code;
	white_instruction_id_ = white_instruction_id;
	white_pin_ = white_pin;
	grow_instruction_code_ = grow_instruction_code;
	grow_instruction_id_ = grow_instruction_id;
	grow_pin_ = grow_pin;
	door_pin_ = door_pin;
	door_instruction_code_ = door_instruction_code;
	door_instruction_id_ = door_instruction_id;
}

void Plant3_Lighting::begin(void) {
	if (door_instruction_code_ != "") {
		// do not init door hardware. assume its done somewhere else.
		pinMode(door_pin_, INPUT_PULLUP);
	}
	pinMode(white_pin_, OUTPUT);
	pinMode(grow_pin_, OUTPUT);
	turnOnWhite(0);
	turnOnGrow(0);
	status_ = ERROR_NONE;
	door_opened = false;
}

String Plant3_Lighting::get(int type) {
	// Initialize Message
	String message = "";
	door_value_ = getDoorData();

	//if (door_value_ == 1) {
	//	// door open
	//	turnOnGrow(0); //turn off grow lights
	//	turnOnWhite(100);
	//}
	//else {
	//	turnOnWhite(white_value_);
	//	turnOnGrow(grow_value_); 
	//}

	//Serial.println("DOOR code");
	//Serial.println(door_instruction_code_);
	// Append Actuator State
	if (door_instruction_code_ != "") {
		message += "\"";
		message += door_instruction_code_;
		message += " ";
		message += door_instruction_id_;
		message += "\":";
		if (type == DATA_MSG) message += door_value_;
		else message += status_;
		message += ",";
	} // else door will be reported somewhere else

	message += "\"";
	message += white_instruction_code_;
	message += " ";
	message += white_instruction_id_;
	message += "\":";
	if (type == DATA_MSG) message += white_value_;
	else message += status_;
	message += ",";

	message += "\"";
	message += grow_instruction_code_;
	message += " ";
	message += grow_instruction_id_;
	message += "\":";
	if (type == DATA_MSG) message += grow_value_;
	else message += status_;
	message += ",";

	return message;
}

String Plant3_Lighting::set(String instruction_code, int instruction_id, String instruction_parameter) {
	if ((instruction_code == white_instruction_code_) && (instruction_id == white_instruction_id_)) {
		int temp = instruction_parameter.toInt();
		if ( (temp >= 0) && (temp <=100) ) {
			turnOnWhite(temp);
		}
		if (temp > 100) turnOnWhite(100);
	}

	if ((instruction_code == grow_instruction_code_) && (instruction_id == grow_instruction_id_)) {
		int temp = instruction_parameter.toInt();
		if ((temp >= 0) && (temp <= 100)) {
			turnOnGrow(temp);
		}
		if (temp > 100) turnOnGrow(100);
	}

	return "";
}

//-------------------------------------------------PRIVATE-------------------------------------------//
void Plant3_Lighting::setDefaults(void)
{
}

bool Plant3_Lighting::getDoorData(void) {
	return digitalRead(door_pin_);
}

String Plant3_Lighting::getValue(String data, char separator, int index)
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

int Plant3_Lighting::turnOnWhite(int percent) {
	double pwm = percent * 2.55;
	if (pwm > 255) pwm = 255;
	analogWrite(white_pin_, (int)pwm);
	white_value_ = percent;
	return(percent);
}

int Plant3_Lighting::turnOnGrow(int percent) {
	double pwm = percent * 2.55;
	if (pwm > 255) pwm = 255;
	grow_value_ = (int)pwm;
	analogWrite(grow_pin_, (int)pwm); // active low design to match
	grow_value_ = percent;
	return(percent);
}


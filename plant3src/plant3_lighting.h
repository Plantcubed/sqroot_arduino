#/** 
 *  \file illum_panel.h
 *  \brief Illumination Planel Control
 */

 // Library based off: This file is part of the Adafruit NeoPixel library.
 // Modified by: Richard Klosinski, Plantcubed
 // Date: May 4, 2016

#ifndef PLANT3_LIGHTING_H
#define PLANT3_LIGHTING_H

#include <Arduino.h>

#include "module_handler.h"

#define DEFAULT_GROW_INT 100
#define DEFAULT_WHITE_INT 100

 /**
 *  \brief Illumination Planel Control
 */
class Plant3_Lighting : SensorActuatorModule {
public:
	// Public Functions
	Plant3_Lighting(String white_instruction_code, int white_instruction_id, int white_pin, String grow_instruction_code, int grow_instruction_id, int grow_pin, String door_instruction_code, int door_instruction_id, int door_pin);
	void begin(void);
	String get(int type);
	String set(String instruction_code, int instruction_id, String instruction_parameter);

	bool door_value_;

	int white_value_;

	int grow_value_;

private:
	typedef enum {
		LEDOFF = 0,
		LEDON = 1,
	};


	void setDefaults(void);
	bool getDoorData(void);
	void colorRedBlue(int red, int blue);
	String getValue(String data, char separator, int index);
	int turnOnWhite(int percent);
	int turnOnGrow(int percent);

	bool door_opened;
	int previous_uv_state;
	int previous_panel_state;

	String
		white_instruction_code_,
		grow_instruction_code_,
		door_instruction_code_;

	int
		EE_Address_,
		white_instruction_id_,
		grow_instruction_id_,
		white_pin_,
		grow_pin_,
		door_instruction_id_,
		door_pin_;

	uint32_t status_;


};

#endif PLANT3_LIGHTING_H


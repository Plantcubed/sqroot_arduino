/**
*  \file actuator_hbridge.h
*  \brief Actuator module for A3909G hbridge driver.
*  \authorRIchard Klosinski
*/

#ifndef ACTUATOR_PH_H
#define ACTUATOR_PH_H

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "module_handler.h"

/**
* \brief Actuator module for A3909G hbridge driver.
*/
class Plant3_ActuatorPH : SensorActuatorModule {
	//------------------------------------------------PUBLIC---------------------------------------------//
public:
	/* Public Functions */
	/**
	* \brief Used to construct an instance of the ActuatorA3909GHBridgee class.
	* @param[in] pin is the control line for the relay
	* @param[in] instruction_code is the 4-letter instruction
	* code associated with the actuation the relay is switching. Instruction
	* codes are not necessarily unique.
	* @param[in] instruction_id is the ID associated with the specific
	* actuator that is being switched. Each ID is unique.
	*
	* To clarify how parameters are used, consider the following case.
	* Imagine there are two air heaters each connected to their own relay. They would
	* have the same instruction code: AAHE (Actuator Air Heater) but would both
	* need to be addressable. Thus the instruction_ids of the heaters would be
	* 1 and 2.
	*/
	Plant3_ActuatorPH(String instruction_code, int instruction_id, int up_pinA, int up_pinB, int down_pinA, int down_pinB, int forwarddirection, unsigned long timeout);

	/**
	* \brief Called once to setup module.
	* Sets specified control pin to be in OUTPUT mode.
	*/
	void begin(void);

	/**
	* \brief Returns JSON string with module data.
	* Data: "<instruction_code> <instruction_id> <value>",
	* Example: "AAHU 1 1",
	*/
	String get(int type);

	/**
	* \brief Sets relay to be on / off.
	* If instruction_code & instruction_id match the code and id
	* module was instantiated with, the relay will enter the state
	* passed in through instruction_parameter.
	* If instruction_parameter = "1", relay is ON (switch closed).
	* If instruction_paremerter = "0", relay is OFF (switch open).
	*/
	String set(String instruction_code, int instruction_id, String instruction_parameter);

	// Public Variables
	int value_;

	//------------------------------------------------PUBLIC---------------------------------------------// 
private:
	// Private Functions
	void UpForward(void);
	void UpReverse(void);
	void DownForward(void);
	void DownReverse(void);
	void StopUp(void);
	void StopDown(void);
	void StopAll(void);

	// Private Variables
	int
		down_pinA_,
		down_pinB_,
		up_pinA_,
		up_pinB_;
	int instruction_id_;
	unsigned long timeout_;
	unsigned long last_read_time_;
	String instruction_code_;
	uint32_t status_;
};

#endif ACTUATOR_HBRIDGE_H


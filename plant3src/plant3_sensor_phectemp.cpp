/** 
 *  \file sensor_dfr0161_0300.cpp
 *  \brief Sensor module for water ph, ec, and temperature. 
 *  \details See sensor_dfr0161_0300.h for details.
 *  \author Richard Klosinski
 */
#include "plant3_sensor_phectemp.h"
#include "error.h"
#include <EEPROM.h>

//------------------------------------------PUBLIC FUNCTIONS----------------------------------------//
Plant3SensorPhECTemp::Plant3SensorPhECTemp(int ph_pin, String ph_instruction_code, int ph_instruction_id, int temperature_pin, String temperature_instruction_code, int temperature_id, int ec_pin, int ec_control_pin, String ec_instruction_code, int ec_id) {
  ph_pin_ = ph_pin;
  ph_instruction_code_ = ph_instruction_code;
  ph_instruction_id_ = ph_instruction_id;
  temperature_pin_ = temperature_pin;
  temperature_instruction_code_ = temperature_instruction_code;
  temperature_id_ = temperature_id;
  ec_pin_ = ec_pin;
  ec_control_pin_ = ec_control_pin;
  ec_instruction_code_ = ec_instruction_code;
  ec_id_ = ec_id;
}

void Plant3SensorPhECTemp::begin() {

  EE_Address_ = Get_EE_AddressPtr();
  //Serial.println("PhEC_EE_Address_");
  //Serial.println(EE_Address_);

  // Construct Objects
  ds_ = new OneWire(temperature_pin_);
  ph_filter_ = new MovingAverageFilter(5);
  ec_filter_ = new MovingAverageFilter(5);
  temperature_filter_ = new MovingAverageFilter(5);
  
  // Configure Initial State
  pinMode(ec_control_pin_, OUTPUT);
  digitalWrite(ec_control_pin_, LOW); // turn EC off
  last_update_was_ec_ = true;
  prev_update_time_ = millis();

  // Set Configuration Parameters
  ec_on_delay_ = 400; // milliseconds
  ec_off_delay_ = 400; // milliseconds
  measuerment_mode_ = DEFAULT_NORMAL;

  EEPROM.get(EE_Address_, CalConfig);

  // point to next spot
  Set_EE_AddressPtr(EE_Address_ + sizeof(CalConfig) + 1);
  ignore_temp_ = 0;
  // get the checksum;
  uint16_t checksum = checksumEE();
  //Serial.println("PhEC calc checksum");
  //Serial.println(checksum);
  //Serial.println("PhEC stored checksum");
  //Serial.println(CalConfig.checksum);

  // Set Calibration Parameters
  if (CalConfig.checksum != checksum) {
	  //Serial.println("Setting phec Calibration Data");
	  setPhDefaults();
	  setECDefaults();
	  PutEE();
  }

  status_ = ERROR_NONE;
  //Serial.println("PhEC cal/config");
  //Serial.println(CalConfig.EC_Slope);
  //Serial.println(CalConfig.PH_Offset);
  //Serial.println(CalConfig.PH_Slope);
  //Serial.println(CalConfig.Measurement_Samples);
  //Serial.println(CalConfig.checksum);
}

String Plant3SensorPhECTemp::get(int type) {
  // Get PH, Temperature, & EC Data
  getSensorData();

  // Initialize Message
  String message = "";

  // Append PH
  message += "\"";
  message += ph_instruction_code_;
  message += " ";
  message += ph_instruction_id_;
  message += "\":";
  if (type == DATA_MSG) message += String(ph_filtered,1);
  else message += status_;
  message += ",";

  // Append Temperature
  message += "\"";
  message += temperature_instruction_code_;
  message += " ";
  message += temperature_id_;
  message += "\":";
  if (type == DATA_MSG) message += String(temperature_filtered,1);
  else message += status_;
  message += ",";

  // Append EC
  message += "\"";
  message += ec_instruction_code_;
  message += " ";
  message += ec_id_;
  message += "\":";
  if (type == DATA_MSG) message += String(ec_filtered,1);
  else message += status_;
  message += ",";

  // Return Message
  return message;
}

String Plant3SensorPhECTemp::set(String instruction_code, int instruction_id, String instruction_parameter)
{
	String temp;

	String smode = getValue(instruction_parameter, ' ', 0);
	int mode = smode.toInt();
	if ((instruction_code == ph_instruction_code_) && (instruction_id == ph_instruction_id_)) {
		//Serial.println("Mode = " + mode);
		switch (mode) {
			case SET_SAMPLES:
				temp = getValue(instruction_parameter, ' ', 1);
				//Serial.println("Set Samples " + temp);
				CalConfig.Measurement_Samples = temp.toInt();
				PutEE();
				break;

			case CALIBRATE_7_0:
				//Serial.println("Cal PH");
				CalPh();
				break;

			case RESET:
				setPhDefaults();
				PutEE();
				break;

			case RAW:
				temp = getValue(instruction_parameter, ' ', 1);
				measuerment_mode_ = temp.toInt();
				//Serial.println("Measurement Mode " + temp);
				break;

			case LIST:
				Serial.println("PH Config/Cal");
				Serial.println(CalConfig.PH_Offset);
				Serial.println(CalConfig.PH_Slope);
				Serial.println(CalConfig.Measurement_Samples);
				Serial.println(CalConfig.checksum);
				break;
		}
	}

	if ((instruction_code == ec_instruction_code_) && (instruction_id == ec_id_)) {
		switch (mode) {
			case SET_SAMPLES:
				temp = getValue(instruction_parameter, ' ', 1);
				//Serial.println("Set Samples " + temp);
				CalConfig.Measurement_Samples = temp.toInt();
				PutEE();
				break;

			case CALIBRATE_1_413:
				//Serial.println("Cal EC low");
				CalEc(CALIBRATE_1_413);
				break;

			case CALIBRATE_12_880:
				//Serial.println("Cal EC high");
				CalEc(CALIBRATE_12_880);
				break;

			case RESET:
				// write persistant data
				setECDefaults();
				PutEE();
				break;

			case RAW:
				temp = getValue(instruction_parameter, ' ', 1);
				measuerment_mode_ = temp.toInt();
				//Serial.println("Measurement Mode " + temp);
				break;

			case IGNORE_TEMP:
				 temp = getValue(instruction_parameter, ' ', 1);
				 ignore_temp_ = temp.toInt();
				break;

			case LIST:
				float ec_slope = (1.413 - 12.883) / (CalConfig.X_1_413 - CalConfig.X_12_883);
				float ec_offset = 1.413 - (CalConfig.X_1_413 * ec_slope);
				Serial.println("CalConfig.X_1_413");
				Serial.println(CalConfig.X_1_413);
				Serial.println("CalConfig.X_12_883");
				Serial.println(CalConfig.X_12_883);
				Serial.println("EC offset");
				Serial.println(ec_offset);
				Serial.println("EC slope ");
				Serial.println(ec_slope);
				Serial.println(CalConfig.Measurement_Samples);
				Serial.println(CalConfig.checksum);
				break;

		}
	}
  return "";
}

//------------------------------------------PRIVATE FUNCTIONS----------------------------------------//
void Plant3SensorPhECTemp::PutEE()
{
	// write the data into eeprom
	EEPROM.put(EE_Address_, CalConfig);

	unsigned int checksum = checksumEE();
	CalConfig.checksum = checksum;
	// write the checksum
	EEPROM.put(EE_Address_, CalConfig);
}

int Plant3SensorPhECTemp::checksumEE(void)
{
	int checksum = 0;	

	for (int address = 0; address < (sizeof(CalConfig) - sizeof(unsigned int)); address++) {
		checksum += EEPROM.read(address + EE_Address_);
	}
	return checksum;
}

void Plant3SensorPhECTemp::setPhDefaults(void)
{
	//Serial.println("Reset PH to defaults");
	CalConfig.PH_Offset = 2.1252;
	CalConfig.PH_Slope = 1.9656;
	CalConfig.Measurement_Samples = DEFAULT_SAMPLES;
}

void Plant3SensorPhECTemp::setECDefaults(void)
{
	//Serial.println("Reset EC to defaults");
	CalConfig.X_1_413 = 1.58;
	CalConfig.X_12_883 = 3.45;
	CalConfig.Measurement_Samples = DEFAULT_SAMPLES;
}

void Plant3SensorPhECTemp::getSensorData(void) {
  // Update Temperature
  temperature_raw = getTemperature();
  temperature_filtered = (float)round(temperature_filter_->process(temperature_raw)*10)/10; // set accuracy to +-0.05
  
  // Alternate Updating pH & EC With Enforced Delays & ec enable/disable
  if (last_update_was_ec_ && (millis() - prev_update_time_ > ec_off_delay_)) {
    // Update pH
    ph_raw = getPh();
	ph_filtered = (float)round(ph_filter_->process(ph_raw)*10)/10; // set accuracy to +-0.05
    //digitalWrite(ec_control_pin_, HIGH); // enable EC for next reading period
    prev_update_time_ = millis();
    last_update_was_ec_ = false;
  }
  else if (!last_update_was_ec_ && (millis() - prev_update_time_ > ec_on_delay_)) {
    // Update EC
    ec_raw = getEc(temperature_filtered);
	//ec_filtered = ec_raw;
    ec_filtered = (float)round(ec_filter_->process(ec_raw)*10)/10; // set accuracy to +-0.05
    //dgitalWrite(ec_control_pin_, LOW);  // disable EC for pH reading.
    prev_update_time_ = millis();
    last_update_was_ec_ = true;
  }

  if (ph_filtered < 11.0) status_ &= ~PH_PROBE;
  else status_ |= PH_PROBE;

  if (temperature_filtered > 2.0) {
	  status_ &= ~WTEMP_PROBE;
  }
  else {
	  status_ |= WTEMP_PROBE;
	  ignore_temp_ = 1;
  }

}

float Plant3SensorPhECTemp::getPh(void) {
  // Sampling Specifications
  int analog_sum = 0;

  // Acquire Samples
  for (int i = 0; i<CalConfig.Measurement_Samples; i++) {
	  analog_sum += analogRead(ph_pin_);
  }

  float analog_average = (float)analog_sum / CalConfig.Measurement_Samples;
  float volts = analog_average*(float)5.0 / 1024;

  //int * voltage;
  //voltage = new int[CalConfig.Measurement_Samples];

  // Acquire Samples
  //f//or (int i=0; i<CalConfig.Measurement_Samples; i++) {
    //voltage[i] = analogRead(ph_pin_);
  //}

  // Remove Min & Max Samples, Average, Convert to Voltage
  //float volts = avergeArray(voltage, CalConfig.Measurement_Samples)*5.0/1024;

  if (measuerment_mode_ != DEFAULT_NORMAL) return volts;

  // Convert Average Voltage to pH
  float ph = CalConfig.PH_Slope*volts + CalConfig.PH_Offset;
  if (ph < 0) ph = 0;
  return ph;
}

int Plant3SensorPhECTemp::CalPh(void)
{
	// ph calibration is done at pH of 7
	// get reading
	// Sampling Specifications
	int * voltage; 
	const float calibration_ph = 7.0;
	voltage = new int[25];

	// Acquire Samples
	for (int i = 0; i<25; i++) {
		voltage[i] = analogRead(ph_pin_);
	}

	// Remove Min & Max Samples, Average, Convert to Voltage
	double volts = avergeArray(voltage, 25)*5.0 / 1024;

	// calculate new slope
	float new_offset = calibration_ph - (volts * CalConfig.PH_Slope);

	// calculate new slope
	//Serial.println("New Offset");
	//Serial.println(new_offset);
	//Serial.println("Old Offset");
	//Serial.println(CalConfig.PH_Offset);

	// write new value of offset
	CalConfig.PH_Offset = new_offset;
	PutEE();
}

float Plant3SensorPhECTemp::getTemperature(void) {
  float temperature_value;
  // Read Temperature
  byte present = ds_->reset();
  ds_->select(temperature_address_);
  ds_->write(0xBE); // Read Scratchpad            
  for (int i = 0; i < 9; i++) { // we need 9 bytes
	temperature_data_[i] = ds_->read();
  }         
  ds_->reset_search();
  byte MSB = temperature_data_[1];
  byte LSB = temperature_data_[0];        
  float tempRead = ((MSB << 8) | LSB); // using two's compliment
  temperature_value = tempRead / 16;

  // Start Conversion For Next Temperature Reading
  if ( !ds_->search(temperature_address_)) {
     // Serial.println("no more sensors on chain, reset search!");
      ds_->reset_search();
  }      
  if ( OneWire::crc8(temperature_address_, 7) != temperature_address_[7]) {
     //Serial.println("CRC is not valid!");
  }        
  if ( temperature_address_[0] != 0x10 && temperature_address_[0] != 0x28) {
     //Serial.print("Device is not recognized!");
  }      
  ds_->reset();
  ds_->select(temperature_address_);
  ds_->write(0x44,1); // start conversion, with parasite power on at the end

  // Return Temperature
  return temperature_value;
}

float Plant3SensorPhECTemp::getEc(float temperature_value) {
  int analog_sum = 0;
  
  for (int i=0; i<CalConfig.Measurement_Samples; i++) {
    analog_sum += analogRead(ec_pin_);
  }

  float analog_average = (float) analog_sum / CalConfig.Measurement_Samples;
  float voltage = analog_average*(float)5.0/1024;

  float temperature_coefficient;
  if (ignore_temp_ == 0 ) temperature_coefficient = 1.0 + 0.0185*(temperature_value - 25.0);
  else temperature_coefficient = 1.0;  // test only

  if (measuerment_mode_ != DEFAULT_NORMAL) return  voltage;

  // calculate slope
  float ec_slope = (1.413 - 12.883) / (CalConfig.X_1_413 - CalConfig.X_12_883);
  float ec_offset = 1.413 - (CalConfig.X_1_413 * ec_slope);
  float ec = (ec_slope* voltage * temperature_coefficient) + ec_offset;
  if (ec < 0) ec = 0;
  return ec;
}

int Plant3SensorPhECTemp::CalEc(int calval)
{
	// ec calibration is done at ec of both 1.413uS and  12.883uS
	// get reading
	// Sampling Specifications
	int voltage[25];

	// Acquire Samples
	for (int i = 0; i<25; i++) {
		voltage[i] = analogRead(ec_pin_);
	}

	// Remove Min & Max Samples, Average, Convert to Voltage
	double volts = avergeArray(voltage, 25)*5.0 / 1024;

	// write new value of offset
	if (calval == CALIBRATE_12_880) {
		CalConfig.X_12_883 = volts;
	}
	else {
		CalConfig.X_1_413 = volts;
	}
	PutEE();
}

float Plant3SensorPhECTemp::avergeArray(int* arr, int number){
  int i;
  int max,min;
  float avg;
  long amount=0;
  if(number<=0){
    //Serial.println("Error number for the array to averaging!/n");
    return 0;
  }
  if(number<5){   //less than 5, calculated directly statistics
    for(i=0;i<number;i++){
      amount+=arr[i];
    }
    avg = amount/number;
    return avg;
  }else{
    if(arr[0]<arr[1]){
      min = arr[0];max=arr[1];
    }
    else{
      min=arr[1];max=arr[0];
    }
    for(i=2;i<number;i++){
      if(arr[i]<min){
        amount+=min;        //arr<min
        min=arr[i];
      }else {
        if(arr[i]>max){
          amount+=max;    //arr>max
          max=arr[i];
        }else{
          amount+=arr[i]; //min<=arr<=max
        }
      }//if
    }//for
    avg = (float)amount/(number-2);
  }//if
  return avg;
}

String Plant3SensorPhECTemp::getValue(String data, char separator, int index)
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



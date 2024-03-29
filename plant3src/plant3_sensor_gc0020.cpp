/** 
 *  \file plant3_sensor_gc0022.cpp
 *  \brief Sensor module for air co2, temperature, and humidity.
 *  \details See sensor_gc0022.h for details.
 */
#include "plant3_sensor_gc0020.h"
#include "error.h"

//------------------------------------------------PUBLIC---------------------------------------------//
Plant3_SensorGc0020::Plant3_SensorGc0020(int rx_pin, int tx_pin, String co2_instruction_code, int co2_instruction_id, String temperature_instruction_code, int temperature_instruction_id,String humidity_instruction_code, int humidity_instruction_id) {
  rx_pin_ = rx_pin;
  tx_pin_ = tx_pin;
  co2_instruction_code_ = co2_instruction_code;
  co2_instruction_id_ = co2_instruction_id;
  temperature_instruction_code_ = temperature_instruction_code;
  temperature_instruction_id_ = temperature_instruction_id;
  humidity_instruction_code_ = humidity_instruction_code;
  humidity_instruction_id_ = humidity_instruction_id;
  timeout_ = 40; // milliseconds
}

void Plant3_SensorGc0020::begin(void) {
  if (NULL != ss_) {
    delete ss_;
  }
  ss_ = new SoftwareSerial(rx_pin_,tx_pin_);
  ss_->begin(9600);
  delay(100);
  String message;
  sendMessage("K 2"); //set sensor to polling mode
  message = receiveMessage();
  sendMessage("A 32"); //set sensor to default digital filtering value
  message = receiveMessage();
  ss_->end();
}

String Plant3_SensorGc0020::get(int type) {
  // Get Sensor Data
  getSensorData();

  // Initialize Message
  String message = "";
  
  // Append CO2 Data to Message
  message += "\"";
  message += co2_instruction_code_;
  message += " ";
  message += co2_instruction_id_;
  message += "\":";
  if ( type == DATA_MSG) message += String(co2, 0);
  else  message += status_;
  message += ",";
 
  // Append Temperature Data to Message
  message += "\"";
  message += temperature_instruction_code_;
  message += " ";
  message += temperature_instruction_id_;
  message += "\":";
  if (type == DATA_MSG) message += String(temperature, 1);
  else message += status_;
  message += ",";

  // Append Humidity Data to Message
  message += "\"";
  message += humidity_instruction_code_;
  message += " ";
  message += humidity_instruction_id_;
  message += "\":";
  if (type == DATA_MSG) message += String(humidity, 1);
  else message += status_;
  message += ",";
  
  return message;
}

String Plant3_SensorGc0020::set(String instruction_code, int instruction_id, String instruction_parameter) {
  String return_message = "";
  // Check Instruction Code and ID Match
  if ((instruction_code == co2_instruction_code_) && (instruction_id == co2_instruction_id_)) {
    // Check for Calibration Command
    int len = instruction_parameter.length();
    if (len >= 3) {
      if (instruction_parameter[0] == 'C') { // Calibration Command
        ss_->begin(9600);
        sendMessage(instruction_parameter.substring(2, len));
        delay(20);
        String response = receiveMessage();
        if (response != "") {
          return_message += "\"";
          return_message += co2_instruction_code_;
          return_message += " ";
          return_message += co2_instruction_id_;
          return_message += "\":\"";
          return_message += response.substring(0,response.length()-2);
          return_message += "\",";
        }
        ss_->end();
      }
    }
  }
  return return_message;
}

//------------------------------------------------PRIVATE--------------------------------------------------//
void Plant3_SensorGc0020::getSensorData(void) {
  // Reset Values
  co2 = 0;
  temperature = 0;
  humidity = 0;
  
  // Open Connection
  ss_->begin(9600);

  // Get CO2
  String message;
  sendMessage("Z");
  message = receiveMessage();
  if(message[1] == 'Z') {
    co2 = (float)(message.substring(3,8).toInt());
    co2 = round(co2/10)*10;
	status_ &= ~CO2_SENSOR;
  }
  else status_ |= CO2_SENSOR;

  // Get Temperature
  sendMessage("T");
  message = receiveMessage();
  if(message[1] == 'T') {
    temperature = 0.1*(float)(message.substring(3,8).toInt()-1000);
	status_ &= ~CO2_SENSOR;
  }
  else status_ |= CO2_SENSOR;
  
  // Get Humidity
  sendMessage("H");
  message = receiveMessage();
  if(message[1] == 'H') {
    humidity = 0.1*(float)(message.substring(3,8).toInt());
	status_ &= ~CO2_SENSOR;
  }
  else status_ |= CO2_SENSOR;

  // Close Connection
  ss_->end();
}

void Plant3_SensorGc0020::sendMessage(String message) {
  message += "\r\n";
  ss_->print(message);
}

String Plant3_SensorGc0020::receiveMessage(void) {
  String message="";
  char incoming_char;
  uint32_t start_time = millis();
  while (millis() - start_time < timeout_){
    if (ss_->available()) {
      incoming_char = ss_->read();
      message += incoming_char;
      if (incoming_char == '\n') {
        break;
      }
    }
  }
  return message;
}



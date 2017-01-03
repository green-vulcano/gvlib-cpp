/*
 * Copyright (c) 2015, GreenVulcano Open Source Project. All rights reserved.
 *
 * This file is part of the GreenVulcano Communication Library for IoT.
 *
 * This is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * This software is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this software. If not, see <http://www.gnu.org/licenses/>.
 */

#include <gvlib_arduino.h>

#include <SPI.h>
#include <Ethernet.h>

using namespace gv;
using gv::CallbackParam;

const uint8_t myIp_[] = {10, 100, 80, 32};
const uint8_t serverIp_[] = {10, 100, 60, 103};
const int port = 1883;
const char device_id[] = "GVDEV999";
const char device_name[] = "GV Device Test";
byte mac[] = { 0xFA, 0x5F, 0x67, 0x5F, 0xBD, 0x85 }; 


/****************************************************
 * Callback for basic device operation
 ***************************************************/
gv::CallbackParam cbDevice(gv::CallbackParam payload) {
  StaticJsonBuffer<128> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject((const char*) payload.data);
  
  if (root["value"] == "ON") {
    modality = 1;
  } else if (root["value"] == "OFF") {
    modality = 0;
  }
}

/*****************************************************
   GVLIB callback functions: they have to respect the
   prototype `CallbackParam function (CallbackParam p)`
   You need to register these functions in the
   `setup()` phase.
******************************************************/

gv::CallbackParam callback1(gv::CallbackParam payload) {
  Serial.print(F("ARDUINO CALLBACK 1: "));
  Serial.println((char*)payload.data);
  return payload;
}

gv::CallbackParam callback2(gv::CallbackParam  payload) {
  Serial.print(F("ARDUINO CALLBACK 2: "));
  Serial.println((char*)payload.data);
  return payload;
}

/****************************************************
   GVLIB initialization: prefer static, so you 
   get little or no surprises (e.g. compared to when
   using "dynamic" memory.
*****************************************************/
IPAddr myIp (myIp_);
IPAddr serverIp (serverIp_);
DeviceInfo deviceInfo(device_id, device_name, myIp, port);
EthernetClient ethClient;

avr::arduino::MqttTransport mqttTransport(deviceInfo, serverIp, port, ethClient);
avr::DefaultProtocol protocol(deviceInfo, mqttTransport);
GVComm gvComm(deviceInfo, mqttTransport, protocol);


/****************************************************
 *  Arduino standard setup function
 ****************************************************/
void setup() {
  Serial.begin(115200);
  
  /*****************************
   * Enable ethernet first!
   *****************************/
  Ethernet.begin(mac, myIp.v4());
  
  /*****************************
   * Connect the MQTT transport
   *****************************/
  mqttTransport.connect();
  
  /*****************************
   * Then you declare yourself
   *****************************/
  Serial.println(F("Sending Device Information: "));
  gvComm.addDevice(cbDevice);

  /*************************************
   * Then you declare your sensors...
   *************************************/
  Serial.println(F("Sending Sensors Configuration: "));
  gvComm.addSensor("SED99901", "Sensor TEST", "NUMERIC");  

  /*************************************
   * ... and actuators
   * 
   * Don't forget to register your callbacks!
   * (you can do this in the beginning too,
   *  but ALWAYS after Ethernet is initialized)
   *************************************/
  Serial.println(F("Sending Actuators Configuration: "));
  gvComm.addActuator("ACD99901", "Actuator TEST", "NUMERIC", callback1);
  gvComm.addActuator("ACD99902", "Actuator TEST", "NUMERIC", callback2);
}


/****************************************************
 *  Arduino standard loop function
 ****************************************************/
void loop() {
  Serial.println(F("LOOPING..."));
  gvComm.poll();
}

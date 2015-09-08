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

const uint8_t myIp_[] = { 10,100,80,99 };
byte mac[] = { 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA };
const uint8_t serverIp_[] = { 10,100,60,103 };
const int port = 1883;
const char device_id[] = "GVDEV999";
const char device_name[] = "GV Basic Actuator";

/****************************************************
 * Actuator
 ****************************************************/
int pinLed = 6;

/*****************************************************
   GVLIB callback functions: they have to respect the
   prototype `CallbackParam function (CallbackParam p)`
   You need to register these functions in the
   `setup()` phase.
******************************************************/

gv::CallbackParam light(gv::CallbackParam payload) {

  char* value = (char*)payload.data;
  Serial.print(F("LED Status: "));
  Serial.println(value);

  if (strcmp(value,"on")==0) {
    digitalWrite(pinLed, HIGH);
  }
  if (strcmp(value,"off")==0) {
    digitalWrite(pinLed, LOW);
  }
    
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
   * Enable pin 
   *****************************/ 
  pinMode(pinLed, OUTPUT);

  /*****************************
   * Connect the MQTT transport
   *****************************/
  mqttTransport.connect();

  /*****************************
   * Then you declare yourself
   *****************************/
  Serial.println(F("Sending Device Information: "));
  gvComm.addDevice();

  /*************************************
   * Then you declare your actuators...
   * 
   * Don't forget to register your callbacks!
   * (you can do this in the beginning too,
   *  but ALWAYS after Ethernet is initialized)
   *************************************/
  Serial.println(F("Sending Actuators Configuration: "));
  gvComm.addActuator("ACD99901", "Actuator TEST", "STRING", light);

  Serial.println("SETUP COMPLETED");
}

/****************************************************
 *  Arduino standard loop function
 ****************************************************/
void loop() {
  
  //Serial.println(F("LOOPING..."));
  gvComm.poll();
}


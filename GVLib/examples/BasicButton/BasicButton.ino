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
const char device_name[] = "GV Basic Sensor";

/****************************************************
 * Sensor
 ****************************************************/
int pinButton = 2;
int stateButton = LOW;
int oldStateButton = LOW;

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
  pinMode(pinButton, INPUT);
  
  /*****************************
   * Connect the MQTT transport
   *****************************/
  mqttTransport.connect();

  /*****************************
   * Then you declare yourself
   *****************************/
  Serial.println(F("Sending Device Information: "));
  gvComm.sendDeviceInfo();

  /*************************************
   * Then you declare your sensors...
   *************************************/
  Serial.println(F("Sending Sensor Configuration: "));
  gvComm.sendActuatorConfig(1, "Button TEST", "NUMERIC", "/devices/GVDEV999/sensors/SED99901");

  Serial.println("SETUP COMPLETED");

}

/****************************************************
 *  
 ****************************************************/

void manageButton() {
  stateButton = !digitalRead(pinButton);
  
  if (stateButton == HIGH && oldStateButton == LOW) {
    gvComm.sendData(pinButton, "{\"value\":true}");
    oldStateButton = HIGH;
  }
  else if(stateButton == LOW && oldStateButton == HIGH) {
    oldStateButton = LOW;
  }
}

/****************************************************
 *  Arduino standard loop function
 ****************************************************/

void loop() {
  
  //Serial.println(F("LOOPING..."));

  manageButton();
  
  gvComm.poll();
}


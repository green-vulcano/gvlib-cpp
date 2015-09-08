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
#include <ArduinoJson.h>

#include <SPI.h>
#include <Ethernet.h>
#include <Servo.h>

using namespace gv;
using gv::CallbackParam;

const uint8_t myIp_[] = { 10,100,80,28 };
byte mac[] = { 0xDE, 0x1D, 0xBC, 0xEE, 0xFE, 0xED };
const uint8_t serverIp_[] = { 10,100,60,103 };
const int port = 1883;
const char device_id[] = "GVDEV001";
const char device_name[] = "GV Robotic Hand";

/****************************************************
 * Actuators
 ****************************************************/
Servo servo_thumb;
Servo servo_index_finger;
Servo servo_middle_finger;
Servo servo_ring_finger;
Servo servo_little_finger;

int pin_thumb = 7;
int pin_index_finger = 6;
int pin_middle_finger = 5;
int pin_ring_finger = 4;
int pin_little_finger = 3;

/*****************************************************
   GVLIB callback functions: they have to respect the
   prototype `CallbackParam function (CallbackParam p)`
   You need to register these functions in the
   `setup()` phase.
******************************************************/

gv::CallbackParam thumb(gv::CallbackParam payload) {
  Serial.print(F("Position Thumb: "));
  Serial.println((char*)payload.data);
  int value = getServoValue((byte*)payload.data);
  handle_thumb(value,false);
  return payload;
}

gv::CallbackParam index_finger(gv::CallbackParam payload) {
  Serial.print(F("Position Index finger: "));
  Serial.println((char*)payload.data);
  int value = getServoValue((byte*)payload.data);
  handle_index_finger(value, false);
  return payload;
}

gv::CallbackParam middle_finger(gv::CallbackParam payload) {
  Serial.print(F("Position Middle finger: "));
  Serial.println((char*)payload.data);
  int value = getServoValue((byte*)payload.data);
  handle_middle_finger(value, true);
  return payload;
}

gv::CallbackParam ring_finger(gv::CallbackParam payload) {
  Serial.print(F("Position Ring finger: "));
  Serial.println((char*)payload.data);
  int value = getServoValue((byte*)payload.data);
  handle_ring_finger(value, false);
  return payload;
}

gv::CallbackParam little_finger(gv::CallbackParam payload) {
  Serial.print(F("Position Little finger: "));
  Serial.println((char*)payload.data);
  int value = getServoValue((byte*)payload.data);
  handle_little_finger(value, true);
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

  Ethernet.begin(mac, myIp.v4());
  
  servo_thumb.attach(pin_thumb);
  servo_index_finger.attach(pin_index_finger);
  servo_middle_finger.attach(pin_middle_finger);
  servo_ring_finger.attach(pin_ring_finger);
  servo_little_finger.attach(pin_little_finger);

  mqttTransport.connect();

  Serial.println(F("Sending Device Information: "));
  gvComm.sendDeviceInfo();

  Serial.println(F("Sending Actuators Configuration: "));
  gvComm.sendActuatorConfig("ACD00101", "Servo Thumb", "NUMERIC");
  gvComm.sendActuatorConfig("ACD00102", "Servo Index Finger", "NUMERIC");
  gvComm.sendActuatorConfig("ACD00103", "Servo Middle Finger", "NUMERIC");
  gvComm.sendActuatorConfig("ACD00104", "Servo Ring Finger", "NUMERIC");
  gvComm.sendActuatorConfig("ACD00105", "Servo Little Finger", "NUMERIC");

  gvComm.addCallback("ACD00101", thumb);
  gvComm.addCallback("ACD00102", index_finger);
  gvComm.addCallback("ACD00103", middle_finger);
  gvComm.addCallback("ACD00104", ring_finger);
  gvComm.addCallback("ACD00105", little_finger);
  
//  gvComm.addCallback("/devices/GVDEV001/actuators/ACD00101/input", thumb);
//  gvComm.addCallback("/devices/GVDEV001/actuators/ACD00102/input", index_finger);
//  gvComm.addCallback("/devices/GVDEV001/actuators/ACD00103/input", middle_finger);
//  gvComm.addCallback("/devices/GVDEV001/actuators/ACD00104/input", ring_finger);
//  gvComm.addCallback("/devices/GVDEV001/actuators/ACD00105/input", little_finger);
  
  Serial.println("SETUP COMPLETED");
}

/****************************************************
 *  Arduino standard loop function
 ****************************************************/
void loop() {
  //Serial.println(F("LOOPING..."));
  gvComm.poll();
}

/****************************************************
 *  
 ****************************************************/

int getServoValue(byte* payload) {
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject((char*)payload);

  return (int)root["value"];
}

/****************************************************
 *  
 ****************************************************/

void handle_thumb(int value, boolean reverse) {
  if(reverse) {
    value = (int)(-1*(value - 180));
  }
  servo_thumb.write(value);
}

/****************************************************
 *  
 ****************************************************/
void handle_index_finger(int value, boolean reverse) {
  if(reverse) {
    value = (int)(-1*(value - 180));
  }
  servo_index_finger.write(value);
}

/****************************************************
 *  
 ****************************************************/
void handle_middle_finger(int value, boolean reverse) {
  if(reverse) {
    value = (int)(-1*(value - 180));
  }
  servo_middle_finger.write(value);
}

/****************************************************
 *  
 ****************************************************/
void handle_ring_finger(int value, boolean reverse) {
  if(reverse) {
    value = (int)(-1*(value - 180));
  }
  servo_ring_finger.write(value);
}

/****************************************************
 *  
 ****************************************************/
void handle_little_finger(int value, boolean reverse) {
  if(reverse) {
    value = (int)(-1*(value - 180));
  }
  servo_little_finger.write(value);
}


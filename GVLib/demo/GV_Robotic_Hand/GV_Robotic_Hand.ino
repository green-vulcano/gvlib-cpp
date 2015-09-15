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
 * We define a simple wrapping class for our servos
 * and their related info
 ****************************************************/

#define F_THUMB     0
#define F_INDEX     1
#define F_MIDDLE    2
#define F_RING      3
#define F_LITTLE    4
#define NUM_FINGERS 5

struct ServoDesc {
    // in a real-life example you'd want to make this
    // a bit more robust, but for the demo's it's probably
    // more than sufficient
    const char* id;
    const char* name;
    const int   pin;
    Servo       servo;
    
    ServoDesc(const char* id_, const char* name_, int pin_,
	      gv::CallbackPointer callback, GVComm& gvComm)
	: id(id_), name(name_), pin(pin_)
    {    
	servo.attach(pin);
	gvComm.addActuator(id, name, "NUMERIC", callback);
    }
    
    void move(int value, bool reverse)
    {
	if(reverse) {
	    value = (-1*(value - 180));    
	}
	Serial.print(F("Moving ")); Serial.print(name);
	Serial.print(F(" to: "));   Serial.println(value);
	servo.write(value);
    }
};

ServoDesc* arrServoDesc[NUM_FINGERS]; // will be initialized in setup()

int extractValueFromPayload(void* payload) {
  StaticJsonBuffer<128> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject((const char*) payload);
  return (int)root["value"];
}

/*****************************************************
   GVLIB callback functions: they have to respect the
   prototype `CallbackParam function (CallbackParam p)`
   You need to register these functions in the
   `setup()` phase.
******************************************************/
gv::CallbackParam cbFinger(gv::CallbackParam payload) {
  arrServoDesc[payload.param]->move(extractValueFromPayload(payload.data), false);
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

  mqttTransport.connect();

  Serial.println(F("Sending Device Information: "));
  gvComm.addDevice();

  Serial.println(F("Sending Actuators Configuration: "));
  
  arrServoDesc[F_THUMB ] = new ServoDesc("ACD00101",  "Servo Thumb",  7, CallbackDescriptor(cbFinger, F_THUMB),  gvComm);
  arrServoDesc[F_INDEX ] = new ServoDesc("ACD00102",  "Servo Index",  6, CallbackDescriptor(cbFinger, F_INDEX),  gvComm);
  arrServoDesc[F_MIDDLE] = new ServoDesc("ACD00103",  "Servo Middle", 5, CallbackDescriptor(cbFinger, F_MIDDLE), gvComm);
  arrServoDesc[F_RING  ] = new ServoDesc("ACD00104",  "Servo Ring",   4, CallbackDescriptor(cbFinger, F_RING),   gvComm);
  arrServoDesc[F_LITTLE] = new ServoDesc("ACD00105",  "Servo Little", 3, CallbackDescriptor(cbFinger, F_LITTLE), gvComm);
  
  Serial.println("SETUP COMPLETED");
}

/****************************************************
 *  Arduino standard loop function
 ****************************************************/
void loop() {
  //Serial.println(F("LOOPING..."));
  gvComm.poll();
}



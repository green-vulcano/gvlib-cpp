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
#include <SoftwareSerial.h>

using namespace gv;
using gv::CallbackParam;

const uint8_t myIp_[] = {10, 100, 80, 31};
byte mac[] = { 0xFA, 0x5F, 0x67, 0x5F, 0xBD, 0x85 }; 
const uint8_t serverIp_[] = {10, 100, 60, 103};
const int port = 1883;
const char device_id[] = "GVDEV002";
const char device_name[] = "GV Glove";

/****************************************************
 * SoftwareSerial for debug
 ****************************************************/
SoftwareSerial BTserial(2, 3); // RX | TX 

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
void setup() 
{
    Serial.begin(9600);
    BTserial.begin(9600);  
    
    Ethernet.begin(mac, myIp.v4());
    mqttTransport.connect();
    
    Serial.print(F("Sending Device Information: "));
    gvComm.addDevice();
    
    Serial.print(F("Sending Sensors Configuration: "));
    gvComm.addSensor("SED00201", "Flex Thumb", "NUMERIC");
    gvComm.addSensor("SED00202", "Flex Index Finger", "NUMERIC");
    gvComm.addSensor("SED00203", "Flex Middle Finger", "NUMERIC");
    gvComm.addSensor("SED00204", "Flex Ring Finger", "NUMERIC");
    gvComm.addSensor("SED00205", "Flex Little Finger", "NUMERIC");
    
    Serial.println("Setup completed");
}

/****************************************************
 *  Arduino standard loop function
 ****************************************************/
void loop() 
{
    if (Serial.available() > 0) {
      BTserial.print("Value received: ");
      BTserial.println(Serial.parseInt());
      
      int value = Serial.parseInt();
      String finger = deduceFinger(value);
      
      StaticJsonBuffer<200> jsonBuffer;
      JsonObject& root = jsonBuffer.createObject();
      char buffer[128];
      char* data(buffer);
      
      switch(finger.toInt()) {
	case 1:
	  value = value - 1000;
	  root["value"] = value;
	  root.printTo(buffer, sizeof(buffer));  
	  gvComm.sendData("SED00201", data);
	  break;
	case 2:
	  value = value - 2000;
	  root["value"] = value;
	  root.printTo(buffer, sizeof(buffer));  
	  gvComm.sendData("SED00202", data);
	  break;
	case 3:
	  value = value - 3000;
	  root["value"] = value;
	  root.printTo(buffer, sizeof(buffer));  
	  gvComm.sendData("SED00203", data);
	  break;
	case 4:
	  value = value - 4000;
	  root["value"] = value;
	  root.printTo(buffer, sizeof(buffer));  
	  gvComm.sendData("SED00204", data);
	  break;
	case 5:
	  value = value - 5000;
	  root["value"] = value;
	  root.printTo(buffer, sizeof(buffer));  
	  gvComm.sendData("SED00205", data);
	  break;  
      }
      
    } else {
      BTserial.println("Serial is not available");
      delay(1000);
    }
    
    gvComm.poll();
}

/****************************************************
 * 
 ****************************************************/
String deduceFinger(int value) {
  String finger = "";
  finger = ((String)value).substring(0,1);
  return finger;
}

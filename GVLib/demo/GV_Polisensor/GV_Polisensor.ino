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

using namespace gv;
using gv::CallbackParam;

const uint8_t myIp_[] = {10, 100, 80, 32};
byte mac[] = { 0xFA, 0x5F, 0x67, 0x5F, 0xBD, 0x85 }; 
const uint8_t serverIp_[] = {10, 100, 60, 103};
const int port = 1883;
const char device_id[] = "GVDEV003";
const char device_name[] = "GV Polisensor";

/****************************************************
 * Sensors and actuators
 ****************************************************/
const int DELTA = 10;

int pinStatusLed       = A0;
int pinPotenziometer   = A1;
int pinSlider          = A2;
int pinRotary          = A3;

int pinTrigger      = 2;
int pinEcho         = 3;
int pinButtonGreen  = 5;
int pinButtonRed    = 6;
int pinSwitch       = 7;

// DISTANCE SENSOR
long valueDistance = -1;
unsigned long timeDistance = 0;
int MAX_RANGE = 200;
int MIN_RANGE = 0;
int TIMEOUT = 6000;

// GREEN BUTTON
int stateButtonGreen = LOW;
int oldStateButtonGreen = LOW;

// RED BUTTON
int stateButtonRed = LOW;
int oldStateButtonRed = LOW;

// SWITCH
int stateSwitch = LOW;
int oldStateSwitch = LOW;

// POTEZIOMETER
int valuePoteziometer = -1;

// SLIDER
int valueSlider = -1;

// ROTARY
int valueRotary = -1;
unsigned long timeRotary = 0;

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

  pinMode(pinTrigger,     OUTPUT);
  pinMode(pinEcho,        INPUT);
  pinMode(pinStatusLed,   OUTPUT);
  pinMode(pinButtonGreen, INPUT);
  pinMode(pinButtonRed,   INPUT);
  pinMode(pinSwitch,      INPUT);

  Ethernet.begin(mac, myIp.v4());

  digitalWrite(pinStatusLed, LOW);
  mqttTransport.connect();
  digitalWrite(pinStatusLed, HIGH);
  
  Serial.print(F("Sending Device Information: "));
  gvComm.sendDeviceInfo();
  Serial.println(F("done."));

  Serial.print(F("Sending Sensors Configuration: "));
  gvComm.sendSensorConfig(pinTrigger,       "Distance Sensor", "NUMERIC");
  gvComm.sendSensorConfig(pinButtonGreen,   "Green Button", "MONOSTABLE");
  gvComm.sendSensorConfig(pinButtonRed,     "Red Button", "MONOSTABLE");
  gvComm.sendSensorConfig(pinSwitch,        "Switch Button", "BISTABLE");
  gvComm.sendSensorConfig(pinPotenziometer, "Potenziometer", "NUMERIC");
  gvComm.sendSensorConfig(pinSlider,        "Slider", "JSON_OBJECT");
  gvComm.sendSensorConfig(pinRotary,        "Rotary", "NUMERIC");
  Serial.println(F("dones."));

  Serial.println("Setup completed");
}

/****************************************************
 *  
 ****************************************************/
void manageDistance() {
  if(millis() >= timeDistance+100) {
    timeDistance = millis();
    
    digitalWrite(pinTrigger, LOW); 
    delayMicroseconds(2); 
    
    digitalWrite(pinTrigger, HIGH);
    delayMicroseconds(10); 
    
    digitalWrite(pinTrigger, LOW);
    long duration = pulseIn(pinEcho, HIGH, TIMEOUT);
    
    //Calculate the distance (in cm) based on the speed of sound.
    long distance = duration/58.2; // (duration/29/2)
  
    if (distance >= MAX_RANGE || distance <= MIN_RANGE){
      // Serial.println("out of range");
    }
    else {
      if(distance != valueDistance) {
        valueDistance = distance;
        
        StaticJsonBuffer<200> jsonBuffer;
        JsonObject& root = jsonBuffer.createObject();      
        
        root["value"] = distance;
        char buffer[256];
        root.printTo(buffer, sizeof(buffer));  
        char* data(buffer);
    
        gvComm.sendData(pinTrigger, data);      
        Serial.println(distance);
      }
    }
  }
}

/****************************************************
 *  
 ****************************************************/
void manageButtonGreen() {
  stateButtonGreen = !digitalRead(pinButtonGreen);
  
  if (stateButtonGreen == HIGH && oldStateButtonGreen == LOW) {
    gvComm.sendData(pinButtonGreen, "{\"value\":true}");
    oldStateButtonGreen = HIGH;
  }
  else if(stateButtonGreen == LOW && oldStateButtonGreen == HIGH) {
    oldStateButtonGreen = LOW;
  }
}

/****************************************************
 *  
 ****************************************************/
void manageButtonRed() {
  stateButtonRed = !digitalRead(pinButtonRed);
  
  if (stateButtonRed == HIGH && oldStateButtonRed == LOW) {
    gvComm.sendData(pinButtonRed, "{\"value\":true}");
    oldStateButtonRed = HIGH;
  }
  else if(stateButtonRed == LOW && oldStateButtonRed == HIGH) {
    oldStateButtonRed = LOW;
  }
}

/****************************************************
 *  
 ****************************************************/
void manageSwitch() {
  stateSwitch = digitalRead(pinSwitch);
  
  if(stateSwitch == HIGH && oldStateSwitch == LOW) {
     gvComm.sendData(pinSwitch, "{\"value\":true}");
  }
  else if(stateSwitch == LOW && oldStateSwitch == HIGH) {
    gvComm.sendData(pinSwitch, "{\"value\":false}");
  }

  oldStateSwitch = stateSwitch;
}

/****************************************************
 *  
 ****************************************************/
void managePotenziometer() {
  int value = int(analogRead(pinPotenziometer) / DELTA) * DELTA;

  if(value != valuePoteziometer) {
    valuePoteziometer = value;
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();      
    
    root["value"] = value;
    char buffer[256];
    root.printTo(buffer, sizeof(buffer));  
    char* data(buffer);
    
    gvComm.sendData(pinPotenziometer, data);    
  }
}


/****************************************************
 *  
 ****************************************************/
int getStep(int value) {
  int step = 0;
      
  if(value>=0 && value<=89) {
    step = -4;
  }
  else if(value>=90 && value<=169) {
    step = -3;
  }
  else if(value>=170 && value<=259) {
    step = -2;
  }
  else if(value>=260 && value<=349) {
    step = -1;
  }
  else if(value>=350 && value<=439) {
    step = 0;
  }
  else if(value>=440 && value<=519) {
    step = 1;
  }
  else if(value>=520 && value<=599) {
    step = 2;
  }
  else if(value>=600 && value<=789) {
    step = 3;
  }
  else if(value>=790 && value<=1023) {
    step = 4;
  }

  return step;
}

/****************************************************
 *  
 ****************************************************/
void manageSlider() {
  int value = int(analogRead(pinSlider) / DELTA) * DELTA;

  if(value != valueSlider) {
    valueSlider = value;
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();      

    int step_value = getStep(value);
    
    root["value"] = value;
    root["step"]  = step_value;
    char buffer[256];
    root.printTo(buffer, sizeof(buffer));  
    char* data(buffer);

    gvComm.sendData(pinSlider, data);    
  }
}

/****************************************************
 *  
 ****************************************************/
void manageRotary() {
  if(millis() > timeRotary+100) {
    timeRotary = millis();
    
    int value = int(analogRead(pinRotary) / DELTA) * DELTA;
  
    if(value != valueRotary) {    
      valueRotary = value;
      StaticJsonBuffer<200> jsonBuffer;
      JsonObject& root = jsonBuffer.createObject();      
      
      root["value"] = value;
      char buffer[256];
      root.printTo(buffer, sizeof(buffer));  
      char* data(buffer);
    
      gvComm.sendData(pinRotary, data);    
    }
  }
}

/****************************************************
 *  Arduino standard loop function
 ****************************************************/
void loop() {
  manageDistance();
  manageButtonGreen();
  manageButtonRed();
  manageSwitch();
  managePotenziometer();
  manageSlider();
  manageRotary();  

  if(!mqttTransport.connected()) {
    digitalWrite(pinStatusLed, LOW);
  }
  else {
    digitalWrite(pinStatusLed, HIGH);
  }
  
  gvComm.poll();
}




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
#include <ArduinoJson.h>

using namespace gv;
using gv::CallbackParam;

const uint8_t myIp_[] = {10, 100, 80, 31};
byte mac[] = { 0xAA, 0x1D, 0xBC, 0xCB, 0xEE, 0xED };
const uint8_t serverIp_[] = {10, 100, 80, 39};
//const uint8_t serverIp_[] = {10, 100, 60, 103};
const int port = 1883;
const char device_id[] = "GVDEV002";
const char device_name[] = "GV Glove";

const int FINGER_THUMB = 1;
const int FINGER_INDEX = 2;
const int FINGER_MIDDLE = 3;
const int FINGER_RING = 4;
const int FINGER_LITTLE = 5;

int check_thumb = -1;
int check_index = -1;
int check_middle = -1;
int check_ring = -1;
int check_little = -1;

int min_value[5] = {0, 0, 0, 0, 0};
int max_value[5] = {0, 0, 0, 0, 0};

int DELTA_VALUE = 25;

const int MIN_VALUE = 0;
const int MAX_VALUE = 180;

int modality = 1;
const char* ON = "ON";
const char* OFF = "OFF";

/****************************************************
 * Callback for basic device operation
 ***************************************************/
gv::CallbackParam cbDevice(gv::CallbackParam payload) {
  StaticJsonBuffer<32> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject((const char*) payload.data);

  Serial.println(F("CALLBACK DEVICE CALLED"));
  const char* root_value = (const char*)root["value"];
  
  if (strcmp(root_value,ON) == 0) {
    Serial.println(F("Mode ON"));
    modality = 1;
  } else if (strcmp(root_value,OFF) == 0) {
    Serial.println(F("Mode OFF"));
    modality = 0;
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

const int buttonPin1 = 2;    // the number of the pushbutton pin
const int buttonPin2 = 4;    // the number of the pushbutton pin
const int ledPin1 = 3;      // the number of the LED pin
const int ledPin2 = 5;      // the number of the LED pin

int min_calibrate = LOW;
int max_calibrate = LOW;

/****************************************************
 *  
 ****************************************************/
int readButton1() {
  return digitalRead(buttonPin1);
}

/****************************************************
 *  
 ****************************************************/
int readButton2() {
  return digitalRead(buttonPin2);
}

/****************************************************
 *
 ****************************************************/
int deduceFinger(int value) {
  return int(value/1000);
}

/****************************************************
 *
 ****************************************************/
int mapValue(int value, int fromMin, int fromMax, int toMin, int toMax) {
  if(value < fromMin) {
    value = fromMin;
  }
  else if(value > fromMax) {
    value = fromMax;
  }
          
  return int(map(value, fromMin, fromMax, toMin, toMax));
}

/****************************************************
 *  
 ****************************************************/
void calibrateMinimum(int value, int finger) {   
  value = int(value/DELTA_VALUE) * DELTA_VALUE;

  if (finger == FINGER_THUMB) {
    value = value - 1000;
    min_value[0] = value;
  }
  else if (finger == FINGER_INDEX) {
    value = value - 2000;
    min_value[1] = value;
  }
  else if (finger == FINGER_MIDDLE) {
    value = value - 3000;
    min_value[2] = value;
  }
  else if (finger == FINGER_RING) {
    value = value - 4000;
    min_value[3] = value;
  }
  else if (finger == FINGER_LITTLE) {
    value = value - 5000;
    min_value[4] = value;
  }

  if(min_value[0]!=0 && min_value[1]!=0 && min_value[2]!=0 && min_value[3]!=0 && min_value[4]!=0) {
    min_calibrate = HIGH;
    digitalWrite(ledPin1, HIGH);
  } 
}

/****************************************************
 *  
 ****************************************************/
void calibrateMaximum(int value, int finger) {   
  value = int(value/DELTA_VALUE) * DELTA_VALUE;
  
  if (finger == FINGER_THUMB) {
    value = value - 1000;
    max_value[0] = value;
  }
  else if (finger == FINGER_INDEX) {
    value = value - 2000;
    max_value[1] = value;
  }
  else if (finger == FINGER_MIDDLE) {
    value = value - 3000;
    max_value[2] = value;
  }
  else if (finger == FINGER_RING) {
    value = value - 4000;
    max_value[3] = value;
  }
  else if (finger == FINGER_LITTLE) {
    value = value - 5000;
    max_value[4] = value;
  }

  if(max_value[0]!=0 && max_value[1]!=0 && max_value[2]!=0 && max_value[3]!=0 && max_value[4]!=0) {
    max_calibrate = HIGH;
    digitalWrite(ledPin2, HIGH);
  }
}

/****************************************************
 *
 ****************************************************/
void moveFinger(int value, int finger) {
  char* sensor_id = "";
  value = int(value/DELTA_VALUE) * DELTA_VALUE;
  
  if (finger == FINGER_THUMB && (value > check_thumb+DELTA_VALUE || value < check_thumb-DELTA_VALUE)) {
    check_thumb = value;
    sensor_id = "SED00201\0";
    value = value - 1000;
    value = mapValue(value, min_value[0], max_value[0], MIN_VALUE, MAX_VALUE);
  }
  else if (finger == FINGER_INDEX && (value > check_index+DELTA_VALUE || value < check_index-DELTA_VALUE)) {
    check_index = value;
    sensor_id = "SED00202\0";
    value = value - 2000;
    value = mapValue(value, min_value[1], max_value[1], MIN_VALUE, MAX_VALUE);
  }
  else if (finger == FINGER_MIDDLE && (value > check_middle+DELTA_VALUE || value < check_middle-DELTA_VALUE)) {
    check_middle = value;
    sensor_id = "SED00203\0";
    value = value - 3000;
    value = mapValue(value, min_value[2], max_value[2], MIN_VALUE, MAX_VALUE);
  }
  else if (finger == FINGER_RING && (value > check_ring+DELTA_VALUE || value < check_ring-DELTA_VALUE)) {
    check_ring = value;
    sensor_id = "SED00204\0";
    value = value - 4000;
    value = mapValue(value, min_value[3], max_value[3], MIN_VALUE, MAX_VALUE);
  }
  else if (finger == FINGER_LITTLE && (value > check_little+DELTA_VALUE || value < check_little-DELTA_VALUE)) {
    check_little = value;
    sensor_id = "SED00205\0";
    value = value - 5000;
    value = mapValue(value, min_value[4], max_value[4], MIN_VALUE, MAX_VALUE);
  }
  else {
    value = -1;
  }    

  if (value >= 0) {
    char buf[8] = {'\0'};
    char res[32] = {'\0'};
    itoa(value, buf, 10);

    strcat(res, "{\"value\":");
    strcat(res, buf);
    strcat(res, "}");

    gvComm.sendData(sensor_id, res);
  }
}

/****************************************************
 *  Arduino standard setup function
 ****************************************************/
void setup() {
  pinMode(buttonPin1, INPUT);
  pinMode(buttonPin2, INPUT);
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  
  Serial.begin(9600);
  Ethernet.begin(mac, myIp.v4());
  mqttTransport.connect();

  gvComm.addDevice(cbDevice);
  gvComm.addSensor("SED00201", "Flex Thumb", "NUMERIC");
  gvComm.addSensor("SED00202", "Flex Index Finger", "NUMERIC");
  gvComm.addSensor("SED00203", "Flex Middle Finger", "NUMERIC");
  gvComm.addSensor("SED00204", "Flex Ring Finger", "NUMERIC");
  gvComm.addSensor("SED00205", "Flex Little Finger", "NUMERIC");
}

/****************************************************
 *  Arduino standard loop function
 ****************************************************/
void loop() {
    
  if (Serial.available() > 0 && modality == 1) {
    int value = Serial.parseInt();
    int finger = deduceFinger(value);    

    if(min_calibrate == HIGH && max_calibrate == HIGH) {
      moveFinger(value, finger);
    }
    else if(min_calibrate == LOW && readButton1()) {
      calibrateMinimum(value, finger);
    }
    else if(max_calibrate == LOW && readButton2()) {
      calibrateMaximum(value, finger);
    }
  }

  gvComm.poll();
}






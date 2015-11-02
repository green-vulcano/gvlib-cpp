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

//StaticJsonBuffer<200> jsonBuffer;

const uint8_t myIp_[] = {10, 100, 80, 31};
byte mac[] = { 0xFA, 0x5F, 0x67, 0x5F, 0xBD, 0x85 };
//const uint8_t serverIp_[] = {10, 100, 80, 39};
const uint8_t serverIp_[] = {10, 100, 60, 103};
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

int DELTA_VALUE = 1;

int modality = 1;
const char* ON = "ON";
const char* OFF = "OFF";


/****************************************************
 * SoftwareSerial for debug
 ****************************************************/
//SoftwareSerial BTserial(2, 3); // RX | TX

/****************************************************
 * Callback for basic device operation
 ***************************************************/
gv::CallbackParam cbDevice(gv::CallbackParam payload) {
  StaticJsonBuffer<128> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject((const char*) payload.data);

  Serial.println("CALLBACK DEVICE CALLED");
  const char* root_value = (const char*)root["value"];
  
  if (strcmp(root_value,ON) == 0) {
    Serial.println("Modality 1");
    modality = 1;
  } else if (strcmp(root_value,OFF) == 0) {
    Serial.println("Modality 0");
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

/****************************************************
 *  Arduino standard setup function
 ****************************************************/
void setup()
{
  Serial.begin(9600);
  //  BTserial.begin(9600);
  Ethernet.begin(mac, myIp.v4());
  mqttTransport.connect();

  gvComm.addDevice();
  gvComm.addSensor("SED00201", "Flex Thumb", "NUMERIC");
  gvComm.addSensor("SED00202", "Flex Index Finger", "NUMERIC");
  gvComm.addSensor("SED00203", "Flex Middle Finger", "NUMERIC");
  gvComm.addSensor("SED00204", "Flex Ring Finger", "NUMERIC");
  gvComm.addSensor("SED00205", "Flex Little Finger", "NUMERIC");
}

/****************************************************
 *  Arduino standard loop function
 ****************************************************/
void loop()
{
  if (Serial.available() > 0) {
    int value = Serial.parseInt();
    int finger = deduceFinger(value);
    char* sensor_id = "";

    value = int(value/DELTA_VALUE) * DELTA_VALUE;

    if (finger == FINGER_THUMB && value != check_thumb) {
      check_thumb = value;
      sensor_id = "SED00201\0";
      value = value - 1000;
    }
    else if (finger == FINGER_INDEX && value != check_index) {
      check_index = value;
      sensor_id = "SED00202\0";
      value = value - 2000;
    }
    else if (finger == FINGER_MIDDLE && value != check_middle) {
      check_middle = value;
      sensor_id = "SED00203\0";
      value = value - 3000;
    }
    else if (finger == FINGER_RING && value != check_ring) {
      check_ring = value;
      sensor_id = "SED00204\0";
      value = value - 4000;
    }
    else if (finger == FINGER_LITTLE && value != check_little) {
      check_little = value;
      sensor_id = "SED00205\0";
      value = value - 5000;
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

  gvComm.poll();
}

/****************************************************
 *
 ****************************************************/
int deduceFinger(int value) {
  return int(value / 1000);
}

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

//#include <SoftwareSerial.h>

/****************************************************
 * SoftwareSerial for debug
 ****************************************************/
//SoftwareSerial BTserial(2, 3); // RX | TX

/****************************************************
 * Sensors and actuators
 ****************************************************/
int thumbPin       = A0;
int indexPin       = A1;
int middlePin      = A2;
int ringPin        = A3;
int littlePin      = A4;

const int DELTA_VALUE = 1;
int value_thumb    = -1;
int value_index    = -1;
int value_middle   = -1;
int value_ring     = -1;
int value_little   = -1;

const int DELTA_TIME = 200;
unsigned long time_thumb = 0;
unsigned long time_index = 0; 
unsigned long time_middle = 0;
unsigned long time_ring = 0;
unsigned long time_little = 0;

/****************************************************
 *  Arduino standard setup function
 ****************************************************/
void setup() {
  Serial.begin(9600);
}

/****************************************************
 *  Arduino standard loop function
 ****************************************************/
void loop() {
  // manage the data
  sendData(filterValue(thumbPin));
  sendData(filterValue(indexPin));
  sendData(filterValue(middlePin));
  sendData(filterValue(ringPin));
  sendData(filterValue(littlePin));
}

/************************************************
 * Send data function
 ************************************************/
void sendData(int data) {
  if(data > 0) {
    Serial.println(data);
  }
}

/************************************************
 *  Function which it increases the payload of
 *  a significant value and sends the data on
 *  the serial depending on the assigned pin.
 *
 *  In this example the data traveling from the
 *  glove toward a receiver, connected with
 *  GreenVulcano, via bluetooth
 ************************************************/
int filterValue(int pin) {
  int flexPosition = int(analogRead(pin) / DELTA_VALUE) * DELTA_VALUE;  
  int payload = 0;
  unsigned long current_time = millis();

    if((current_time >= time_thumb + DELTA_TIME) && (pin == thumbPin) && (flexPosition != value_thumb)) {
      value_thumb = flexPosition;
      payload = flexPosition + 1000;
      time_thumb = current_time;
    }       
    else if((current_time >= time_index + DELTA_TIME) && (pin == indexPin) && (flexPosition != value_index)) {
      value_index = flexPosition;
      payload = flexPosition + 2000;      
      time_index = current_time;
    }
    else if((current_time >= time_middle + DELTA_TIME) && (pin == middlePin) && (flexPosition != value_middle)) {
      value_middle = flexPosition;
      payload = flexPosition + 3000;      
      time_middle = current_time;
    }
    else if((current_time >= time_ring + DELTA_TIME) && (pin == ringPin) && (flexPosition != value_ring)) {
      value_ring = flexPosition;
      payload = flexPosition + 4000;
      time_ring = current_time;
    }
    else if((current_time >= time_little + DELTA_TIME) && (pin == littlePin) && (flexPosition != value_little)) {
      value_little = flexPosition;
      payload = flexPosition + 5000;      
      time_little = current_time;
    }
  
  return payload;
}


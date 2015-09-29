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
 
#include <SoftwareSerial.h>

/****************************************************
 * SoftwareSerial for debug
 ****************************************************/
SoftwareSerial BTserial(2, 3); // RX | TX

/****************************************************
 * Sensors and actuators
 ****************************************************/
int thumbPin       = A0;
int indexPin       = A1;
int middlePin      = A2;
int ringPin        = A3;
int littlePin      = A4;

const int DELTA    = 30;
int value          = -1;
 
/****************************************************
 *  Arduino standard setup function
 ****************************************************/
void setup() 
{
    Serial.begin(9600);
    BTserial.begin(9600);

    BTserial.println("--- Start reading flex sensors values ----");
}

/****************************************************
 *  Arduino standard loop function
 ****************************************************/
void loop() 
{   
    // for debug
    BTserial.print("Value read thumb finger: ");
    BTserial.println(analogRead(thumbPin));
    BTserial.print("Value read index finger: ");
    BTserial.println(analogRead(indexPin));
    BTserial.print("Value read middle finger: ");
    BTserial.println(analogRead(middlePin));
    BTserial.print("Value read ring finger: ");
    BTserial.println(analogRead(ringPin));
    BTserial.print("Value read little finger: ");
    BTserial.println(analogRead(littlePin));
    
    // manage the data 
    filterValue(thumbPin);
    filterValue(indexPin);
    filterValue(middlePin);
    filterValue(ringPin);
    filterValue(littlePin); 
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
void filterValue(int pin) {
  int flexPosition = int(analogRead(pin) / DELTA) * DELTA;
  int payload = 0;
  if (flexPosition != value) {
    value = flexPosition;
    if (pin == thumbPin) {
      payload = flexPosition + 1000;
    } else if (pin == indexPin) {
      payload = flexPosition + 2000;
    } else if (pin == middlePin) {
      payload = flexPosition + 3000;
    } else if (pin == ringPin) {
      payload = flexPosition + 4000;
    } else if (pin == littlePin) {
      payload = flexPosition + 5000;
    }
    Serial.println(payload);
  }
  
}


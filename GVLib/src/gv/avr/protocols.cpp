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

/*
 * gv/avr/protocols.cpp
 *
 *  Created on: Aug 7, 2015
 *      Author: Domenico Barra <eisenach@gmail.com>
 */


#include "gv/avr/protocols.h"
#include "gv/avr/gv.h"

#include <avr/pgmspace.h>

namespace gv { namespace avr {

/**************************************************************************
 * 
 **************************************************************************/
bool Protocol_IOT_v1::addDevice() {
	Buffer b;
	b.add(PSTR("{\"nm\":\""), true);
	b.add(deviceInfo_.name());
	b.add(PSTR("\",\"ip\":\""), true);
	b.add(deviceInfo_.ip());
	b.add(PSTR("\",\"prt\":\""), true);
	b.add((int)deviceInfo_.port());
	b.add(PSTR("\"}"), true);

	const char* payload = b.get();

	char srv[80];
	int srvlen = sprintf_P(srv, PSTR("%s/%s"), GV_DEVICES, deviceInfo_.id());

	return transport_.send(srv, srvlen, b.get(), b.len());
}

/**************************************************************************
 * 
 **************************************************************************/
bool Protocol_IOT_v1::addSensor(const char* id, const char* name, const char* type) {
	Buffer b;
	b.add(PSTR("{\"nm\":\""), true);
	b.add(name);
	b.add(PSTR("\",\"tp\":\""), true);
	b.add(type);
	b.add(PSTR("\"}"), true);

	char srv[80];
	int srvlen = sprintf_P(srv, PSTR("%s/%s%s/%s"), GV_DEVICES, deviceInfo_.id(), GV_SENSORS, id);

	return transport_.send(srv, srvlen, b.get(), b.len());
}

/**************************************************************************
 * 
 **************************************************************************/
bool Protocol_IOT_v1::addActuator(const char* id, const char* name, const char* type, CallbackDescriptor desc) {
	Buffer b_topic;
	b_topic.add(GV_DEVICES);
	b_topic.add("/");
	b_topic.add(deviceInfo_.id());
	b_topic.add(GV_ACTUATORS);
	b_topic.add("/");
	b_topic.add(id);
	b_topic.add(GV_INPUT);

	// subscribes function to topic
	transport_.subscribe(b_topic.get(), desc);

	Buffer b;
	b.add(PSTR("{\"nm\":\""), true);
	b.add(name);
	b.add(PSTR("\",\"tp\":\""), true);
	b.add(type);
	b.add(PSTR("\"}"), true);

	char srv[80];
	int srvlen = sprintf_P(srv, PSTR("%s/%s%s/%s"), GV_DEVICES, deviceInfo_.id(), GV_ACTUATORS, id);
	
	return transport_.send(srv, srvlen, b.get(), b.len());
}

/**************************************************************************
 * 
 **************************************************************************/
bool Protocol_IOT_v1::sendData(const char* id, const char* value) {
	Buffer b;
	b.add(value);

	char srv[80];
	int srvlen = sprintf_P(srv, PSTR("%s/%s%s/%s%s"), GV_DEVICES, deviceInfo_.id(), GV_SENSORS, id, GV_SENSOR_DATA);

	return transport_.send(srv, srvlen, b.get(), b.len());
}

}} // namespace gv::avr

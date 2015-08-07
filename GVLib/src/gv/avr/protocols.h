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
 * arduino/gv_protocols.h
 *
 * Created on: Aug 6, 2015
 *     Author: Domenico Barra <eisenach@gmail.com>
 */

#ifndef GV_AVR_PROTOCOLS_H_
#define GV_AVR_PROTOCOLS_H_

#include "gv/protocols.h"

#include <stdio.h>
#include <avr/pgmspace.h>

namespace gv {

namespace avr {


class Protocol_IOT_v1 : public gv::protocols::Protocol_IOT_v1 {
public:
	Protocol_IOT_v1(const DeviceInfo& info, Transport& transport) :
		gv::protocols::Protocol_IOT_v1(info, transport) { }

	bool sendDeviceInfo() override {
		Buffer b;
		b.add(PSTR("{\"id\": \""), true);
		b.add(deviceInfo_.id());
		b.add(PSTR("\", \"nm\": \""), true);
		b.add(deviceInfo_.name());
		b.add(PSTR("\", \"ip\": \""), true);
		b.add(deviceInfo_.ip());
		b.add(PSTR("\": \"%s\", \"prt\": \""), true);
		b.add((int)deviceInfo_.port());
		b.add(PSTR("\"}"), true);
		const char* payload = b.get();
		int plen = b.len();
		return transport_.send(GV_DEVICES, strlen(GV_DEVICES), payload, plen);
	}

	bool sendSensorConfig(uint8_t id, const char* name, const char* type) override;
	bool sendActuatorConfig(uint8_t id, const char* name, const char* type, const char* topic) override;
	bool sendData(uint8_t id, const char* value) override;

};


}


}



#endif /* GV_AVR_PROTOCOLS_H_ */

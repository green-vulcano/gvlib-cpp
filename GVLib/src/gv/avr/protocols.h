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
 * gv/avr/protocols.h
 *
 * Created on: Aug 6, 2015
 *     Author: Domenico Barra <eisenach@gmail.com>
 */

#ifndef GV_AVR_PROTOCOLS_H_
#define GV_AVR_PROTOCOLS_H_

#include "gv/protocols.h"

namespace gv {
	namespace avr {
		class Protocol_IOT_v1 : public gv::protocols::Protocol_IOT_v1 {
			public:
				Protocol_IOT_v1(const DeviceInfo& info, Transport& transport) :
					gv::protocols::Protocol_IOT_v1(info, transport) { }

				bool addDevice() override;
				bool addSensor(const char* id, const char* name, const char* type) override;
				bool addActuator(const char* id, const char* name, const char* type, CallbackPointer fn) override;
				bool sendData(const char* id, const char* value) override;
		};

		typedef Protocol_IOT_v1 DefaultProtocol;
	}
}

#endif /* GV_AVR_PROTOCOLS_H_ */

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
 * GreenVulcano Communication Library for IoT - C++ for Micro-Controllers
 * gv/protocols.h
 *
 * Created on: Aug 6, 2015
 *     Author: Domenico Barra <eisenach@gmail.com>
 * 
 * This header file contains the definition of generic GreenVulcano protocols.
 */

#ifndef GV_PROTOCOLS_H_
#define GV_PROTOCOLS_H_

#include "gv/gv.h"

namespace gv {

namespace protocols {


class Protocol_IOT_v1 : public Protocol {
public:
	Protocol_IOT_v1(const DeviceInfo& info, Transport& transport) :
		Protocol(info, transport) { }

};


}


}



#endif /* GV_PROTOCOLS_H_ */

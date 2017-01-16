/*****************************************************************************
 *              .:::::.                                                      *
 *             -+  .| :/                                                     *
 *             o ALAN  s                                                     *
 *             .+.`.| +-                __ _   _ __    ___    ___   _ __     *
 *               .:::y+ .....          / _` | | '__|  / _ \  / _ \ | '_ \    *
 *                   /yo++++o/-     . | (_| | | |    |  __/ |  __/ | | | |   *
 *   .::::.        -o/.       /s/+oo/  \__, | |_|     \___|  \___| |_| |_|   *
 *  o  @@ +-      .s-     .-.  -s:      ___ /      _                         *
 * :: @@ @ dyyyyyyso _-:::  `--.++     |___/      | |                        *
 * `o E@I o-      :s-          .s/    __   ___   _| | ___ __ _ _ __   ___    *
 *   .:::-         +o.        `oo`    \ \ / / | | | |/ __/ _` | '_ \ / _ \   *
 *                 .ss+:-..-:+o-       \ V /| |_| | | (_| (_| | | | | (_) |  *
 *           .:::./ho``.::::-`          \_/  \__,_|_|\___\__,_|_| |_|\___/   *
 *         .+ --- +.                                                         *
 *         o  GVT  o                          T e c h n o l o g i e s        *
 *         .+ --- +.                                                         *
 *           -:::-'                                                          * 
 *****************************************************************************
 *     GreenVulcano C++ Libraries
 *     This file: common/protocol_v1.cpp
 *                Implementation of the GreenVulcano IoT Protocol version 1
 *****************************************************************************
 * Copyright (c) 2016, GreenVulcano Open Source Project. All rights reserved.
 *
 * This file is part of the GreenVulcano Integration Platform.
 *
 * This is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * This software is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this software. If not, see <http://www.gnu.org/licenses/>.
 */

#include "gv/protocol_v1.hpp"
#include "gv/util/string_fmt.hpp"
#include <cassert>
#include <string>
#include <cerrno>

namespace gv { 
namespace protocols {

using std::string;
using namespace gv::util;
/**************************************************************************
 * 
 **************************************************************************/
Status Protocol_IOT_v1::addDevice(CallbackDescriptor desc) {

    if(desc != nullptr) {
        string topic = format("/devices/%s/input", deviceInfo_.id().c_str());
        transport_.subscribe(topic, desc);
    }

    string payload = format("{\"nm\":\"%s\",\"ip\":\"%s\",\"prt\":\"%d\"}",
                            deviceInfo_.name().c_str(),
                            deviceInfo_.addr().c_str(),
                            deviceInfo_.port());

	string service = format("/devices/%s", deviceInfo_.id().c_str());
    return transport_.send(service, payload);
}

/**************************************************************************
 * 
 **************************************************************************/
Status Protocol_IOT_v1::addSensor(const string& id, const string& name, const string& type) {
    string payload = format("{\"nm\":\"%s\",\"tp\":\"%s\"}", name.c_str(), type.c_str());
    string service = format("/devices/%s/sensors/%s", deviceInfo_.id().c_str(), id.c_str());

	return transport_.send(service, payload);
}

/**************************************************************************
 * 
 **************************************************************************/
Status Protocol_IOT_v1::addActuator(const string& id, const string& name, const string& type, CallbackDescriptor desc) {

    string topic = format("/devices/%s/actuators/%s/input", deviceInfo_.id().c_str(), id.c_str());
	transport_.subscribe(topic, desc);

    string payload = format("{\"nm\":\"%s\",\"tp\":\"%s\"}", name.c_str(), type.c_str());

	string service = format("/devices/%s/actuators/%d", deviceInfo_.id().c_str(), id.c_str());	
	return transport_.send(service, payload);
}

/**************************************************************************
 * 
 **************************************************************************/
Status Protocol_IOT_v1::send(const Message& msg) {
    switch (msg.type()) {
    case SENSOR_DATA: {
        string service = format("/devices/%s/sensors/%s/data", deviceInfo_.id().c_str(), msg.src_id().c_str());
	    return transport_.send(service, msg.data());
    }
    default: return Status::by_code(-EOPNOTSUPP);
    }
}

}} // namespace gv::protocols

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
 * Copyright (c) 2015-2017, GreenVulcano Open Source Project.
 * All rights reserved.
 *
 * This file is part of the GreenVulcano Communication Library for IoT.
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
 *****************************************************************************
 *     GreenVulcano Communication Library for C++
 *     This file  : gv/mqtt_transport.hpp
 *                  Main header file MQTT Transport.
 *     Created on : Jan 3, 2017.
 *     Author     : Domenico Barra <eisenach@gmail.com>
 ****************************************************************************/

 #ifndef GV_MQTT_TRANSPORT_HPP
 #define GV_MQTT_TRANSPORT_HPP

#include "gv/gv.hpp"
#include <memory>

namespace gv {

class MqttDriver;

class MqttTransport : public Transport, private ServerAndPort {
	public:
		MqttTransport(
            const DeviceInfo& info, const IPAddr& server, uint16_t port,
            void* plat_config,
			const std::string& username="", const std::string& password=""
        ) : ServerAndPort(server, port), deviceInfo_(info),
            username_(username), password_(password),
            plat_config_(plat_config), driver_(create_driver_()) { }

		bool send(const std::string& service, const std::string& payload) override;
		bool connect() override;
		bool connected() override;
		bool disconnect() override;
		bool poll() override;

	private:
		const DeviceInfo&                   deviceInfo_;
		std::string                         username_;
		std::string                         password_;
        void*                               plat_config_;
        std::unique_ptr<MqttDriver>         driver_;

        static std::unique_ptr<MqttDriver>
		       create_driver_(void* plat_config); // defined per-platform.

		MqttTransport(const MqttTransport&);
		MqttTransport& operator=(const MqttTransport&);

		bool handleSubscription(const string& topic, CallbackPointer fn) override;
};

class MqttDriver
public:
	bool connect() = 0;
	bool connected() = 0;
	bool disconnect() = 0;
	bool poll() = 0;
	virtual ~MqttDriver() = default;
}

 #endif
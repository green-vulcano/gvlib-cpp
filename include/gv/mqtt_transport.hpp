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
#include <cstdint>

namespace gv {
namespace trans {
namespace mqtt {

class Driver;
class WillConfig;

enum Qos : std::uint8_t {
	AT_MOST_ONCE ,
	AT_LEAST_ONCE,
	EXACTLY_ONCE
};

struct WillConfig {
	const std::string will_topic;
	const std::string will_message;
	const Qos         will_qos;
	const bool        will_retain;

	static WillConfig for_device(const std::string& device_id) {
        return WillConfig { "/device/"+device_id+"/status",
                            "{\"status\": \"offline\"}",
                            Qos::EXACTLY_ONCE,
							true };
    }
};

class Driver {
public:
	virtual Status connect() = 0;
	virtual bool connected() = 0;
	virtual Status disconnect() = 0;
	virtual Status send(const std::string& service, const std::string& payload) = 0;
	virtual StatusRet<bool> poll() = 0;
	virtual Status handleSubscription(const string& topic, CallbackPointer fn) = 0;
	virtual ~Driver() = default;
};


/**
 * Transport using the MQTT Protocol.
 */
class Transport : public gv::Transport,
	public gv::WithDeviceInfo,
	public gv::WithServerAndPort,
	public gv::WithUsernameAndPassword
{
	public:
		Transport(
            const DeviceInfo& info, const IPAddr& server, uint16_t port,
            void* plat_config, const WillConfig& will_config, //use for_device
			const std::string& username="", const std::string& password=""
        ) : WithDeviceInfo(info), WithServerAndPort(server, port),
			WithUsernameAndPassword(username, password),
			plat_config_(plat_config), will_config_(will_config),
			driver_(create_driver_()) { }
		Status connect() override    { return driver_->connect();    }
		bool connected() override    { return driver_->connected();  }
		Status disconnect() override { return driver_->disconnect(); }
		Status send(const std::string& service, const std::string& payload)
		           override { return driver_->send(service, payload); }
		StatusRet<bool> poll() override { return driver_->poll(); }
	private:
        void*                               plat_config_;
		WillConfig                          will_config_;
        std::unique_ptr<Driver>             driver_;

        std::unique_ptr<Driver> create_driver_(); // defined per-platform.

		Transport(const Transport&);
		Transport& operator=(const Transport&);

		Status handleSubscription(const string& topic, CallbackPointer fn)
			   override { return driver_->handleSubscription(topic, fn); }
};


} // namespace mqtt
} // namespace trans
} // namespace gv
#endif
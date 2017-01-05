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
 *     This file  : gv/noop_transport.hpp
 *                  Main header file for No-Op Transport.
 *     Created on : Jan 3, 2017.
 *     Author     : Domenico Barra <eisenach@gmail.com>
 ****************************************************************************/

 #ifndef GV_NOOP_TRANSPORT_HPP
 #define GV_NOOP_TRANSPORT_HPP

#include "gv/gv.hpp"
#include <memory>

namespace gv {

class NoopTransport : public Transport {
	public:
		NoopTransport() { }

		bool send(const std::string& service, const std::string& payload) override
		{
			if (connected_) {
				last_topic_ = service;
				last_payload_ = payload;
			}
			return connected_;
		}

		bool connect() override { return (connected_ = true); }

		bool connected() override { return connected_; }

		bool disconnect() override { connected_ = false; return true; }
		
		bool poll() override {
			if (!last_payload_.empty()) {
				Callback::call(last_topic_, CallbackParam(last_payload_, 0));
				last_payload_ = last_topic_ = std::string();
				return true;
			}
			return false;
		}

	private:
		bool        connected_;
		std::string last_payload_;
		std::string last_topic_;

		bool handleSubscription(const string& topic, CallbackPointer fn) override {
			Callback::add(topic, fn);
			return true;
		}
};
} // namespace gv


#endif
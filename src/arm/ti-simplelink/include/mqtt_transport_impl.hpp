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
 *     This file  : ti-simplelink/include/mqtt_transport_impl.hpp
 *                  MQTT Transport for TI-SmartLink - header file.
 *     Created on : Jan 4, 2017.
 *     Author     : Domenico Barra <eisenach@gmail.com>
 ****************************************************************************/

#ifndef GV_TI_SIMPLELINK_MQTT_TRANSPORT_IMPL_HPP
#define GV_TI_SIMPLELINK_MQTT_TRANSPORT_IMPL_HPP

#include <string>
#include <cstdint>

namespace gv {
namespace ti_simplelink {
namespace trans {
namespace mqtt {

/**
 * Specific configuration for TI SimpleLink devices
 */
struct PlatConfig {
    unsigned  server_response_secs; //!< Reasonable server response time
    unsigned  keep_alive_secs;       //!< Message KeepAlive time
    unsigned  mqtt_task_priority;   //!< RTOS priority for MQTT RX task
    bool      debug;                //!< Enable debug information
    int (*dbg_print)(const char *fmt, ...); //!< Debug function.
                                    //!< Not passing one will result in
                                    //!< platform default being used.

    template<typename ... Args> void dbg( const char* fmt, Args ... args ) {
        if (debug && dbg_print) {
            dbg_print(fmt, args ...);
        }
    }

};

void set_uart_base(unsigned long base);

}}}} // namespace gv::ti_simplelink::trans::mqtt


#endif
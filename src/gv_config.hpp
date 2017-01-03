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
 *     GreenVulcano Communication Library for C++
 *     This file: gv_config.hpp
 *                Generic platform configurator for the Library.
 *                All compilation units need to include this file *first* 
 *                in order to have specific platform-dependent settings
 *                enabled or disabled.
 *****************************************************************************
 * Copyright (c) 2016, GreenVulcano Open Source Project. All rights reserved.
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
 */


#ifndef GVLIB_PLATCONFIG_H
#define GVLIB_PLATCONFIG_H

// #define DEBUG true



#ifdef DEBUG
#  define GV_DEBUG(msg)   // define for your 
#  define GV_DEBUGLN(msg) // specific platform
#else
#  define NDEBUG
#  define GV_DEBUG(msg)
#  define GV_DEBUGLN(msg)
#endif


#define DEVICE_ID_SIZE 8 // 10
#define DEVICE_NAME_SIZE 20
#define HOST_SIZE 20
#define TOPIC_NAME_SIZE 44

#define MAX_ATTEMPTS 10

#define MAX_ACTUATORS 10



#endif /* GVLIB_PLATCONFIG_H */
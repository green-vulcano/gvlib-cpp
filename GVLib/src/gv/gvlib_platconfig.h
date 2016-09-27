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
 * gvlib_platconfig.h
 *
 * This file is a generic platform configurator for the C++ GreenVulcano Library.
 * All compilation units need to include this file *first* in order to have specific
 * platform-dependent tweaks enabled or disabled.
 */

#ifndef GVLIB_PLATCONFIG_H
 #define GVLIB_PLATCONFIG_H 1

#ifdef ARDUINO_ESP8266_NODEMCU
	#define __GVLIB_USE_PGMSPACE_FROM_ROOT_FOLDER__ 1
	#define __GVLIB_USE_STDLIB_NONISO__ 1
#endif

#endif /* GVLIB_PLATCONFIG_H */
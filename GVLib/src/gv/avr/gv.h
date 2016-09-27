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
 * gv/avr/gv.h
 *
 * Created on: Jul 16, 2015
 *     Author: Domenico Barra <eisenach@gmail.com>
 * 
 * This header file contains the specialization of the library for AVR
 * micro-controllers.
 */

#ifndef GV_AVR_GV_H_
#define GV_AVR_GV_H_

#include "gv/gv.h"


#ifdef __GVLIB_USE_PGMSPACE_FROM_ROOT_FOLDER__
	#include <pgmspace.h>
#else
   #include <avr/pgmspace.h>
#endif


namespace gv {
namespace avr {

class Buffer{
	char buffer_[BUFFER_SIZE];
	int index_;

public:
	Buffer() : index_(0) { }

	const char* get() const { return buffer_; }
	char* get() { return buffer_; }

	void reset() {
		index_ = 0;
		memset(&buffer_[0], 0, sizeof(buffer_));
	}

	size_t len() {
		return index_;
	}

	size_t add(const char* value, bool progmem_=false) {
		if (index_ >= BUFFER_SIZE) {
			return 0;
		}


		char* (*cpy)(char*, const char*) = progmem_ ? my_strcpy_P : strcpy;
		char* (*ncpy)(char*, const char*, size_t) = progmem_ ? strncpy_P : strncpy;
		size_t (*len)(const char*) = progmem_ ? my_strlen_P : strlen;

		size_t length = len(value);
		size_t next = index_ + length;

		if (next < BUFFER_SIZE) {
			cpy(buffer_+index_, value);
			index_ = next;
		} else {
			length = BUFFER_SIZE - index_ - 1;
			ncpy(buffer_+index_, value, length);
			buffer_[BUFFER_SIZE - 1] = '\0';
			index_ = BUFFER_SIZE;
		}
		return length;

	}
	size_t add(int value) {
		char number[10];
		itoa(value, number, 10);
		return add(number);
	}
	size_t add(float value) {
		char number[10];
		dtostrf(value, 5, 2, number);
		return add(number);
	}

	size_t add(const IPAddr& addr) {
		int added = 0;
		if (addr.type() == IPAddr::IPv4) {
			const uint8_t* v4 = addr.v4();
			for (int i=0; i<3; i++) {
				added += add(v4[i]);
				added += add(".");
			}
			added += add(v4[3]);
		} else {
			// TODO: IMPLEMENT IPV6 conversion to string
		}
		return added;
	}

private:
	/* our own versions of strcpy_P and strlen_P, since we need to
	   take their address in order to write more generic code, and they
	   are actually macros on some platforms */
	static char* my_strcpy_P(char* dst, const char* src) {
		return strcpy_P(dst, src);
	}

	static size_t my_strlen_P(const char* src) {
		return strlen_P(src);
	}

};

}
}



#endif /* GV_AVR_GV_H_ */

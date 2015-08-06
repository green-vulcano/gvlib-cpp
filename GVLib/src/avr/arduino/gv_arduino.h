/*
 * gv_arduino.h
 *
 *  Created on: Jul 16, 2015
 *      Author: Domenico Barra <eisenach@gmail.com>
 */

#ifndef GV_ARDUINO_H_
#define GV_ARDUINO_H_

#include "gvlib.h"
#include "../gv_avr.h"

#include <Arduino.h>

namespace gv {
namespace avr {
namespace arduino {

class Buffer : public gv::avr::Buffer {
public:
	using gv::avr::Buffer::add;

	inline size_t add(const __FlashStringHelper *str) {
		return add(reinterpret_cast<const char*>(str), true);
	}

};

}}}



#endif /* GV_ARDUINO_H_ */

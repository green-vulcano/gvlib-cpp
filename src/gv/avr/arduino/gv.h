/*
 * gv_arduino.h
 *
 *  Created on: Jul 16, 2015
 *      Author: Domenico Barra <eisenach@gmail.com>
 */

#ifndef GV_AVR_ARDUINO_GV_H_
#define GV_AVR_ARDUINO_GV_H_

#include "gv/gv.h"
#include "gv/avr/gv.h"

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



#endif /* GV_AVR_ARDUINO_GV_H_ */

/*
 * gv_avr.h
 *
 *  Created on: Jul 16, 2015
 *      Author: Domenico Barra <eisenach@gmail.com>
 */

#ifndef GV_AVR_H_
#define GV_AVR_H_

#include "gvlib.h"
#include <avr/pgmspace.h>


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

		char* (*cpy)(char*, const char*) = progmem_ ? strcpy_P : strcpy;
		char* (*ncpy)(char*, const char*, size_t) = progmem_ ? strncpy_P : strncpy;
		size_t (*len)(const char*) = progmem_ ? strlen_P : strlen;

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

};

}
}



#endif /* GV_AVR_H_ */

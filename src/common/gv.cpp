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
 * gv/gv.cpp
 *
 * Created on: Jun 12, 2015
 *     Author: Mauro Pagano <m.pagano@greenvulcano.com>
 *             Domenico Barra <eisenach@gmail.com>
 * 
 * Implementation of base, general-purpose classes.
 */

#include "gv/gv.hpp"
#include "gv/portable_endian.hpp"
#include "gv/util/to_string.hpp"
#include "gv/util/string_fmt.hpp"

namespace gv {

	using util::to_string;
	using util::format;
	using std::string;

	const uint8_t IPAddr::IPv4_MASK[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xFF, 0xFF };

	/**************************************************************************
	 * 
	 **************************************************************************/
	IPAddr::IPAddr(const uint16_t data[]) {
		const uint16_t *p=data;
		uint16_t *d = data_.ui16;

		for (int i=0; i < 8; i++) {
			*d = htobe16(*p);
			++d; ++p;
		}
	}

	/**************************************************************************
	 * 
	 **************************************************************************/
	IPAddr::IPAddr(const uint8_t data[], Type type) {
		if (type == IPv4) {
			memcpy(data_.ui8, IPv4_MASK, sizeof(IPv4_MASK));
		}

		memcpy(&(data_.ui8)[12], data, 4);
	}


	/**************************************************************************
	 * 
	 **************************************************************************/
	IPAddr::operator string() const
	{
		switch(type()) {
		case IPv4:
			return format("%d.%d.%d.%d", data_.ui8[12], data_.ui8[13], data_.ui8[14], data_.ui8[15]);
		case IPv6:
			return format("%x:%x:%x:%x:%x:%x:%x:%x",
							data_.ui16[0], data_.ui16[1], data_.ui16[2], data_.ui16[3],
							data_.ui16[4], data_.ui16[5], data_.ui16[6], data_.ui16[7]);
		default: return "<unknown>";
		}
	} 

	/**************************************************************************
	 * 
	 **************************************************************************/
	GVComm::GVComm(const DeviceInfo& deviceInfo, Transport& transport, Protocol& protocol) :
		deviceInfo_(deviceInfo), transport_(transport), protocol_ (protocol) { }


	/**************************************************************************
	 * 
	 **************************************************************************/
	Status GVComm::addDevice(CallbackDescriptor desc) { 
		return protocol_.addDevice(desc); 
	}

	/**************************************************************************
	 * 
	 **************************************************************************/
	Status GVComm::addActuator(const string& id, const string& name, const string& type, CallbackDescriptor desc) {
		return protocol_.addActuator(id, name, type, desc);
	}

	/**************************************************************************
	 * 
	 **************************************************************************/
	StatusRet<bool> GVComm::poll() {
		return transport_.poll();
	}

	/**************************************************************************
	 *  Class Callback --- definition 
	 **************************************************************************/
	Callback* Callback::head_ = NULL;

	/**************************************************************************
	 * 
	 **************************************************************************/
	Callback::Callback(const string& topic, CallbackDescriptor desc) :
		topic_(topic), next_(NULL), desc_(desc) { }

	/**************************************************************************
	 * 
	 **************************************************************************/
	Callback* Callback::add(const string& topic, CallbackDescriptor desc) {
		Callback* cb = new Callback(topic, desc);

		if (head_) {
			Callback* ptr = head_;
			while (ptr->next_) ptr = ptr->next_;
			return ptr->next_ = cb;
		}

		return head_ = cb;
	}

	/**************************************************************************
	 * 
	 **************************************************************************/
	int Callback::remove(const string& topic, CallbackPointer fn) {
		Callback *ptr = head_, *prev;
		int removed = 0;

		while (find_(topic, fn, &ptr, &prev)) {
			Callback *bye = ptr;
			ptr = ptr->next_;

			if (prev) { // there is a 'previous' callback instance
				prev->next_ = ptr;
			} 
			else {
				head_ = ptr;
			}

			delete bye;
			++removed;
		}

		return removed;
	}

	/**************************************************************************
	 * 
	 **************************************************************************/
	CallbackParam Callback::call(const string& topic, CallbackParam param) {
		Callback *ptr = head_, *prev;

		while (find_(topic, NULL, &ptr, &prev)) {
			if (ptr->desc_.function) {
				param.param = ptr->desc_.param;
				param = ptr->desc_.function(param);
			}

			ptr = ptr->next_;
		}

		return param;
	}

	/**************************************************************************
	 * 
	 **************************************************************************/
	bool Callback::find_(const string& topic, CallbackPointer fn, Callback** ptr, Callback** prev) {
		Callback* p = *ptr;
		*prev = p;

		while (p) {
			if (p->topic_ == topic) {
				if (fn != NULL && fn != p->desc_.function) {
					continue;
				}

				*ptr = p;
				return true;
			}

			*prev = p;
			p = p->next_;
		}

		return false;
	}

	/**************************************************************************
	 * 
	 **************************************************************************/
	void Callback::dispose() {
		Callback  *bye, *p = head_;

		while (p) {
			bye = p;
			p = p->next_;
			delete bye;
		}

		head_ = NULL;
	}

} //namespace gv

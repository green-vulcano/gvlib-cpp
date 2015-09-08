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

#include "gv/gv.h"
#include "gv/avr/gv.h"
#include "gv/portable_endian.h"

namespace gv {

	const uint8_t IPAddr::IPv4_MASK[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xFF, 0xFF };

	/**************************************************************************
	 * 
	 **************************************************************************/
	IPAddr::IPAddr(const uint16_t data[]) {
		const uint16_t *p=data;
		uint16_t *d = data_.ui16;

		for (int i=0; i < 8; i++) {
			*(d++) = htobe16(*(p++));
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
	GVComm::GVComm(const DeviceInfo& deviceInfo, Transport& transport, Protocol& protocol) :
		deviceInfo_(deviceInfo), transport_(transport), protocol_ (protocol) { }

	/**************************************************************************
	 * 
	 **************************************************************************/
	bool GVComm::addCallback(const char* actuatorId, CallbackPointer fn) {
		gv::avr::Buffer b;
		b.add(GV_DEVICES);
		b.add("/");
		b.add(deviceInfo_.id());
		b.add(GV_ACTUATORS);
		b.add("/");
		b.add(actuatorId);
		b.add(GV_INPUT);

		const char* topic = b.get();

		return transport_.subscribe(topic, fn);
	}

	/**************************************************************************
	 * 
	 **************************************************************************/
	bool GVComm::sendActuatorConfig(const char* id, const char* name, const char* type) {		
		return protocol_.sendActuatorConfig(id, name, type);
	}

	/**************************************************************************
	 * 
	 **************************************************************************/
	bool GVComm::poll() {
		return transport_.poll();
	}

	/**************************************************************************
	 *  Class Callback --- definition 
	 **************************************************************************/
	Callback* Callback::head_ = NULL;

	/**************************************************************************
	 * 
	 **************************************************************************/
	Callback::Callback(const char* topic, CallbackPointer fn) :
		next_(NULL), function_(fn) {

		strncpy(topic_, topic, TOPIC_NAME_SIZE);
	}

	/**************************************************************************
	 * 
	 **************************************************************************/
	Callback* Callback::add(const char* topic, CallbackPointer fn) {
		Callback* cb = new Callback(topic, fn);

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
	int Callback::remove(const char* topic, CallbackPointer fn) {
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
	CallbackParam Callback::call(const char* topic, CallbackParam param) {
		Callback *ptr = head_, *prev;

		while (find_(topic, NULL, &ptr, &prev)) {
			if (ptr->function_) {
				param = ptr->function_(param);
			}

			ptr = ptr->next_;
		}

		return param;
	}

	/**************************************************************************
	 * 
	 **************************************************************************/
	bool Callback::find_(const char* topic, CallbackPointer fn, Callback** ptr, Callback** prev) {
		Callback* p = *ptr;
		*prev = p;

		while (p) {
			if (!strcmp(p->topic_, topic)) {
				if (fn != NULL && fn != p->function_) {
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

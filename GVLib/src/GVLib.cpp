/*
 * GreenVulcano Kit for Arduino
 * Copyright (c) 2015 - GreenVulcano
 */

#include "GVLib.h"

#include <Client.h>
#include <Ethernet.h>
#include <EthernetClient.h>

namespace gv {


	/**************************************************************************
	 * 
	 **************************************************************************/


	GVLib::GVLib(const DeviceInfo& deviceInfo, Transport& transport) :
		_sensorCounter(0),
		transport_(transport),
		deviceInfo_(deviceInfo)
	{
	}


	//******************************************************************************
	//
	//******************************************************************************
	bool GVLib::addCallback(const char* topic, CallbackPointer fn) {
		if (transport_.subscribe(topic)) {
			Callback::add(topic, fn);
			return true;
		}
		return false;
	}


	bool GVLib::poll() {
		return transport_.poll();
	}

	/** 
	 * Class Callback --- definition 
	 */

	GVLib::Callback* GVLib::Callback::head_ = NULL;

	//******************************************************************************
	//
	//******************************************************************************
	GVLib::Callback::Callback(const char* topic, CallbackPointer fn) :
			next_(NULL), topic_(topic), function_(fn)
	{
	}

	//******************************************************************************
	//
	//******************************************************************************
	GVLib::Callback* GVLib::Callback::add(const char* topic, CallbackPointer fn) {
		Callback* cb = new Callback(topic, fn);

		if (head_) {
			Callback* ptr = head_;
			while (ptr->next_) ptr = ptr->next_;
			return ptr->next_ = cb;
		}

		return head_ = cb;
	}

	//******************************************************************************
	//
	//******************************************************************************
	int GVLib::Callback::remove(const char* topic, CallbackPointer fn) {
		Callback *ptr = head_, *prev;
		int removed = 0;
		while (find_(topic, fn, &ptr, &prev)) {
			Callback *bye = ptr;
			ptr = ptr->next_;
			if (prev) { // there is a 'previous' callback instance
				prev->next_ = ptr;
			} else {
				head_ = ptr;
			}
			delete bye;
			++removed;
		}
		return removed;
	}

	//******************************************************************************
	//
	//******************************************************************************
	CallbackParam GVLib::Callback::call(const char* topic, CallbackParam param) {
		Callback *ptr = head_, *prev;
		while (find_(topic, NULL, &ptr, &prev)) {
			if (ptr->function_) param = ptr->function_(param);
			ptr = ptr->next_;
		}
		return param;
	}

	//******************************************************************************
	//
	//******************************************************************************
	bool GVLib::Callback::find_(const char* topic, CallbackPointer fn,
								Callback** ptr, Callback** prev) {
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

	//******************************************************************************
	//
	//******************************************************************************
	void GVLib::Callback::dispose() {
		Callback  *bye, *p = head_;
		while (p) {
			bye = p;
			p = p->next_;
			delete bye;
		}
		head_ = NULL;
	}


} //namespace gv

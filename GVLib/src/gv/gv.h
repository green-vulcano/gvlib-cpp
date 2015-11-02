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
 * gv/gv.h
 *
 * Created on: Jun 12, 2015
 *     Author: Mauro Pagano <m.pagano@greenvulcano.com>
 *             Domenico Barra <eisenach@gmail.com>
 * 
 * Main header of the library.
 * This version of the library is designed for limited-resource devices, and
 * is especially targeted to AVR micro-controllers.
 * Yes, it runs on Arduino, too.
 */

#ifndef GV_GV_H
#define GV_GV_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef DEBUG
#  ifdef ARDUINO
#    include <Arduino.h>
#    define  GV_DEBUG(msg)   Serial.print(msg)
#    define  GV_DEBUGLN(msg) Serial.println(msg)
#  else
#    define GV_DEBUG(msg)
#    define GV_DEBUGLN(msg)
#  endif
#else
#  define GV_DEBUG(msg)
#  define GV_DEBUGLN(msg)
#endif

#define GV_DEVICES      "/devices"
#define GV_SENSORS      "/sensors"
#define GV_ACTUATORS    "/actuators"
#define GV_SENSOR_DATA  "/output"
#define GV_STATUS       "/status"
#define GV_INPUT	"/input"

#define GV_PAYLOAD_STATUS_OFFLINE "{\"st\":false}"
#define GV_PAYLOAD_STATUS_ONLINE "{\"st\":true}"

#define BUFFER_SIZE 160

#define DEVICE_ID_SIZE 10
#define DEVICE_NAME_SIZE 20
#define HOST_SIZE 20
#define TOPIC_NAME_SIZE 44

#define MAX_ATTEMPTS 10

#define MAX_ACTUATORS 10

#ifndef DEBUG_MODE
#define DEBUG_MODE 1
#endif

/*
 TOPIC MQTT
/devices
/devices/<ID_DEVICE>
/devices/<ID_DEVICE>/sensors
/devices/<ID_DEVICE>/sensors/<ID_SENSOR>
/devices/<ID_DEVICE>/actuators
/devices/<ID_DEVICE>/actuators/<ID_ACTAUTOR>
/devices/<ID_DEVICE>/sensors/<ID_SENSOR>/data
/devices/<ID_DEVICE>/status
*/

namespace gv {

/**************************************************************************
 * Class to support both IPv4 and IPv6 address families.
 **************************************************************************/
class IPAddr {
	public:		
		enum Type {
			IPv4, IPv6
		};

		IPAddr(const uint16_t data[]);
		IPAddr(const uint8_t data[], Type type=IPv4);

		const uint8_t* v4() const { 
			return &(data_.ui8)[12]; 
		}

		const uint8_t* v6() const { 
			return data_.ui8; 
		}

		Type type() const {
			return ( memcmp(data_.ui8, IPv4_MASK, sizeof(IPv4_MASK)) != 0 ? IPv6 : IPv4 );
		}

	private:
		union {
			uint8_t  ui8[16];
			uint16_t ui16[8];
		} data_;

		static const uint8_t IPv4_MASK[12];
};

/**************************************************************************
 * 
 **************************************************************************/
class DeviceInfo {
	public:
		DeviceInfo(const char* id, const char* name, const IPAddr& addr, uint16_t port) : 
			id_(id), name_(name), addr_(addr), port_(port) { }
		/* TODO: Low memory on extremely limited chips... only storing pointers for now
		   {
				strncpy(id_, id, DEVICE_ID_SIZE);
				strncpy(name_, name, DEVICE_NAME_SIZE);
				strncpy(ip_, ip, HOST_SIZE);
				port_ = port;
			}
		*/

		const char*    id()   const { return id_;   }
		const char*    name() const { return name_; }
		const IPAddr&  ip()   const { return addr_; }
		uint16_t       port() const { return port_; }

	private:
		/* TODO: Low memory on extremely limited chips... only storing pointers for now
			char id_[DEVICE_ID_SIZE];
			char name_[DEVICE_NAME_SIZE];
			char ip_[HOST_SIZE];
			uint16_t port_;
		*/
		const char     *id_;
		const char     *name_;
		const IPAddr   addr_;
		const uint16_t port_;
};

/**************************************************************************
 * Callback parameter type
 **************************************************************************/ 
struct CallbackParam {
	void*   data;
	size_t  size;
	int     param;
};


/**************************************************************************
 * Callback function type.
 * By convention, if not returning *different* data, a callback
 * function must return the same callback param which it received
 * as input.
 **************************************************************************/
typedef CallbackParam (*CallbackPointer)(CallbackParam);

/**
 * Callback descriptor.
 * This structure carries a callback function pointer and an (optional)
 * integer parameter. Such parameter comes in handy if you use the same
 * callback for multiple topics, in order to distinguish them easily
 * without having to go through string comparison.
 *
 * Implicit conversion to and from `CallbackPointer` is provider for
 * backward compatibility and simplicity (no need to explicitly create
 * a `CallbackDescriptor` if the only thing you want is a `CallbackPointer`.
 */
struct CallbackDescriptor {
	CallbackPointer function;
	int             param;

	/**
	 * Constructor.
	 *
	 * @param  fn    the function to call back.
	 * @param  parm  an optional integer parameter that will be passed to the
	 *               callback function. If not passed, defaults to `0`
	 */
	CallbackDescriptor(CallbackPointer fn, int parm=0)
		: function(fn), param(parm) { }

	/**
	 * Implicit conversion to callback function pointer.
	 */
	operator CallbackPointer() const {
		return function;
	}
};


/**************************************************************************
 *	A class to hold callback delegates in GVLib.
 **************************************************************************/
class Callback {
	public:
		/**
			Adds a new callback to the global chain.
			Creates a new `Callback` instance, sets the corresponding values,
			and returns a pointer to it.
			@param  topic the topic on which this `Callback` shall listen.
			@param  desc  the callback descriptor.
			@return a pointer to the newly created instance (or 0 if the callback
			        was already found into the chain)
		*/
		static Callback* add(const char* topic, CallbackDescriptor desc);

		/**
			Removes a callback from the global chain.
			Removes the `Callback` instance registered for the given topic and
			function pointer (or just for the given topic if no function pointer
			is specified).
			@param  topic the topic for which the callback shall be removed
			@param  fn    the function pointer to remove as a callback. Do not
			              pass it (or pass `NULL`) to remove ALL callbacks for a
						  given topic
			@return the number of callbacks found and removed from the chain.
		*/
		static int remove(const char* topic, CallbackPointer fn=NULL);

		/**
			Invokes the appropriate callbacks.
			Calls all registered callbacks for the given topic.
			@param topic   the topic for which to trigger the callbacks.
			@param param   the parameter to pass to the callback chain.
			               The output of the Nth callback is passed as input
			               to the N+1th.
			@return the data returned by the last callback in the chain for the
			        specific topic.
		*/
		static CallbackParam call(const char* topic, CallbackParam param);
		/**
			Performs clean-up and releases memory.
		*/
		static void dispose();

	private:
		char               topic_[TOPIC_NAME_SIZE];
		//const char*      topic_;
		Callback*          next_;
		CallbackDescriptor desc_;

		static Callback*   head_;

		Callback(const char* topic, CallbackDescriptor desc);

		/**
		 * Finds a `Callback` instance matching the given topic, and (optionally) function pointer.
		 * @param topic the topic to find in the matching instance
		 * @param fn    the specific function pointer to find in the matching instance. If null
		 *              is passed, any function pointer is verified.
		 * @param ptr   pass the address of a `Callback*` variable. If the function returns
		 *              `true`, this will contain the address of the found instance.
		 * @param prev  pass the address of a `Callback*` variable. If the function returns
		 *              `true`, this will contain the address of the instance PREVIOUS TO the
		 *              found instance, or `NULL` if the found instance is the first one
		 *              (i.e. `head` points to it)
		 */
		static bool find_(const char* topic, CallbackPointer fn, Callback** ptr, Callback** prev);
};

/**************************************************************************
 *
 **************************************************************************/
class Transport {
	public:
		virtual bool connected()  = 0;
		virtual bool connect()    = 0;
		virtual bool disconnect() = 0;
		virtual bool send(const char* service, size_t slen, const char* payload, size_t plen) = 0;

		bool subscribe(const char* topic, CallbackDescriptor cb, int param=0) {
			if (handleSubscription(topic, cb)) {
				GV_DEBUG("Subscribe: ");
				GV_DEBUGLN(topic);

				Callback::add(topic, cb);
				return true;
			}
			return false;
		}

		virtual bool poll() = 0;
		virtual ~Transport() { }

	protected:
		Transport() { }
		virtual bool ensureConnected() { if (!connected()) { return connect(); } return true; }

	private:
		virtual bool handleSubscription(const char* topic, CallbackPointer cb) = 0;
		Transport(const Transport&);
		Transport& operator=(const Transport&);
};

/**************************************************************************
 *
 **************************************************************************/
class Protocol {
	public:
		Protocol(const DeviceInfo& info, Transport& transport) :
			transport_(transport), deviceInfo_(info) { }

		virtual bool addDevice(CallbackDescriptor desc=NULL) = 0;
		virtual bool addSensor(const char* id, const char* name, const char* type) = 0;
		virtual bool addActuator(const char* id, const char* name, const char* type, CallbackDescriptor desc) = 0;
		virtual bool sendData(const char* id, const char* value) = 0;

		virtual ~Protocol() { }

	protected:
		Transport&         transport_;
		const DeviceInfo&  deviceInfo_;
};

/**************************************************************************
 *
 **************************************************************************/
class GVComm {
	public:
		GVComm(const DeviceInfo& deviceInfo, Transport& transport, Protocol& protocol);

		// bool addCallback(const char* actuatorId, CallbackPointer fn, int param=0);

		bool addDevice(CallbackDescriptor desc=NULL);
		
		bool addSensor(const char* id, const char* name, const char* type) { 
			return protocol_.addSensor(id, name, type); 
		}

		bool addActuator(const char* id, const char* name, const char* type, CallbackDescriptor desc);

		bool sendData(const char* id, const char* value) { 
			return protocol_.sendData(id, value); 
		}

		bool poll();

		static CallbackParam callback(const char* topic, CallbackParam param) {
			return Callback::call(topic, param);
		}

	private:
		Transport&        transport_;
		Protocol&         protocol_;
		const DeviceInfo& deviceInfo_;
};

/**************************************************************************
 *
 **************************************************************************/
class ServerAndPortTransport_base : public virtual Transport {
	protected:
		//static const int MAX_SERVER_LEN = 64;
		const IPAddr&  server() const { return server_; }
		uint16_t       port()   const { return port_; }

		ServerAndPortTransport_base(const IPAddr& server, uint16_t port) : 
			port_(port), server_(server) { }
			/* TODO: Low memory on extremely limited chips... only storing pointers for now
				{
					strncpy(server_, server, MAX_SERVER_LEN);
				}
			*/
		
	private:
		//char     server_[MAX_SERVER_LEN];
		const IPAddr& server_;
		uint16_t      port_;
};

/* sprintf(statusTopic_, "%s/%s/status", GV_DEVICES, info.id()) */

} // namespace gv

#endif GV_GV_H

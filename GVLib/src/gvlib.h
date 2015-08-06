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
 * This version of the library is designed for limited-resource devices, and
 * is especially targeted to AVR micro-controllers.
 * Yes, it runs on Arduino, too.
 */

#ifndef GVLIB_H
#define GVLIB_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#define GV_DEVICES      "/devices"
#define GV_SENSORS      "/sensors"
#define GV_ACTUATORS    "/actuators"
#define GV_SENSOR_DATA  "/data"
#define GV_STATUS       "/status"

#define GV_PAYLOAD_STATUS_OFFLINE "{\"st\":false}"
#define GV_PAYLOAD_STATUS_ONLINE "{\"st\":true}"

#define BUFFER_SIZE 160

#define DEVICE_ID_SIZE 10
#define DEVICE_NAME_SIZE 20
#define HOST_SIZE 20
#define TOPIC_NAME_SIZE 32

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


/**
 * Class to support both IPv4 and IPv6 address families.
 */
class IPAddr {
public:
	enum Type {
		IPv4, IPv6
	};
	IPAddr(const uint16_t data[]);
	IPAddr(const uint8_t data[], Type type=IPv4);
	const uint8_t* v4() const { return &(data_.ui8)[12]; }
	const uint8_t* v6() const { return data_.ui8; }
	Type type() const {
		return ( memcmp(data_.ui8, IPv4_MASK, sizeof(IPv4_MASK)) != 0 ?
				IPv6 : IPv4 );
	}
private:
	union {
		uint8_t  ui8[16];
		uint16_t ui16[8];
	} data_;
	static const uint8_t IPv4_MASK[12];
};

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


class Transport {
public:
	const DeviceInfo& deviceInfo() { return deviceInfo_; }

	virtual bool connected()  = 0;
	virtual bool connect()    = 0;
	virtual bool disconnect() = 0;

	virtual bool send(const char* service, size_t slen, const char* payload, size_t plen) = 0;

	virtual bool sendConfig() = 0;
	virtual bool sendSensorConfig(uint8_t id, const char* name, const char* type) = 0;
	virtual bool sendActuatorConfig(uint8_t id, const char* name, const char* type, const char* topic) = 0;
	virtual bool sendData(uint8_t id, uint32_t value) = 0;

	virtual bool subscribe(const char* topic) = 0;
	virtual bool poll() = 0;

	virtual ~Transport() { }

protected:
	Transport(const DeviceInfo info) : deviceInfo_(info) { }
	virtual void ensureConnected() { while (!connected()) connect(); }
	const DeviceInfo& deviceInfo_;
private:
	Transport(const Transport&);
	Transport& operator=(const Transport&);
};


class Protocol {
public:
	Protocol(const DeviceInfo& info, Transport& transport) :
		transport_(transport), deviceInfo_(info) { }

	virtual bool sendDeviceInfo() = 0;
	virtual bool sendSensorConfig(uint8_t id, const char* name, const char* type) = 0;
	virtual bool sendActuatorConfig(uint8_t id, const char* name, const char* type, const char* topic) = 0;
	virtual bool sendData(uint8_t id, const char* value) = 0;

	virtual ~Protocol() { }
protected:
	Transport&         transport_;
	const DeviceInfo&  deviceInfo_;
};


/**
 * Callback parameter type
 */
struct CallbackParam {
	void* data;
	size_t size;
};

/**
 * Callback function type.
 * By convention, if not returning *different* data, a callback
 * function must return the same callback param which it received
 * as input.
 */
typedef CallbackParam (*CallbackPointer)(CallbackParam);


class GVComm {
	public:
		GVComm(const DeviceInfo& deviceInfo, Transport& transport);

		bool addCallback(const char* topic, CallbackPointer fn);

		bool sendConfig() { return transport_.sendConfig(); }
		bool sendSensorConfig(uint8_t id, const char* name, const char* type)
		{ return transport_.sendSensorConfig(id, name, type); }
		bool sendActuatorConfig(uint8_t id, const char* name, const char* type, const char* topic)
		{ return transport_.sendActuatorConfig(id, name, type, topic); }
		bool sendData(uint8_t id, uint32_t value)
		{ return transport_.sendData(id, value); }

		bool poll();

		static CallbackParam callback(const char* topic, CallbackParam param) {
			return Callback::call(topic, param);
		}

	private:
		uint8_t    _sensorCounter;
		Transport& transport_;
		const DeviceInfo& deviceInfo_;

		/**
			A class to hold callback delegates in GVLib.
		*/
		class Callback {
		public:

			/**
				Adds a new callback to the global chain.
				Creates a new `Callback` instance, sets the corresponding values,
				and returns a pointer to it.
				@param  topic the topic on which this `Callback` shall listen.
				@param  fn    the function to call back.
				@return a pointer to the newly created instance (or 0 if the callback
				        was already found into the chain)
			*/
			static Callback* add(const char* topic, CallbackPointer fn);

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
			//TODO: Low memory on extremely limited chips... only storing pointers for now
			//char             topic_[TOPIC_NAME_SIZE];
			const char*      topic_;
			Callback*        next_;
			CallbackPointer  function_;

			static Callback* head_;

			Callback(const char* topic, CallbackPointer fn);

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

};


class ServerAndPortTransport_base : public virtual Transport {
protected:
	//static const int MAX_SERVER_LEN = 64;

	const IPAddr&  server() const { return server_; }
	uint16_t       port()   const { return port_; }

	ServerAndPortTransport_base(const DeviceInfo& info, const IPAddr& server,
			uint16_t port) : Transport(info), port_(port)
/* TODO: Low memory on extremely limited chips... only storing pointers for now
	{
		strncpy(server_, server, MAX_SERVER_LEN);
	}
*/
	, server_(server) { }
private:
	//char     server_[MAX_SERVER_LEN];
	const IPAddr& server_;
	uint16_t      port_;
};


class MqttTransport_base : public ServerAndPortTransport_base {
protected:
	MqttTransport_base(const DeviceInfo& info, const IPAddr& server,
			uint16_t port) : Transport(info), ServerAndPortTransport_base(info, server, port)
	{
		sprintf(statusTopic_, "%s/%s/status", GV_DEVICES, info.id());
	}
public:
	const char* statusTopic()      const { return statusTopic_; }
	virtual uint8_t statusQos()    const { return 1; }
	virtual uint8_t statusRetain() const { return 0; }
private:
	char statusTopic_[TOPIC_NAME_SIZE];
};


} // namespace gv

#endif GVLIB_H

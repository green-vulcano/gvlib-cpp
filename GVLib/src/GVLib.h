/*
 * GreenVulcano Kit for Microcontrollers
 * Designed for AVR Chipsets (including Arduino)
 * Copyright (c) 2015 - GreenVulcano
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

class GVLib;


class DeviceInfo {
public:
	DeviceInfo(const char* id, const char* name, const char* ip, uint16_t port) :
		id_(id), name_(name), ip_(ip), port_(port) { }
/* TODO: Low memory on extremely limited chips... only storing pointers for now
   {
		strncpy(id_, id, DEVICE_ID_SIZE);
		strncpy(name_, name, DEVICE_NAME_SIZE);
		strncpy(ip_, ip, HOST_SIZE);
		port_ = port;
	}
*/

	const char* id()   const { return id_;   }
	const char* name() const { return name_; }
	const char* ip()   const { return ip_;   }
	uint16_t    port() const { return port_; }

private:
/* TODO: Low memory on extremely limited chips... only storing pointers for now
	char id_[DEVICE_ID_SIZE];
	char name_[DEVICE_NAME_SIZE];
	char ip_[HOST_SIZE];
	uint16_t port_;
*/
	const char *id_;
	const char *name_;
	const char *ip_;
	uint16_t port_;
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


class GVLib {
	public:
		GVLib(const DeviceInfo& deviceInfo, Transport& transport);

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

	const char* server() const { return server_; }
	uint16_t    port()   const { return port_; }

	ServerAndPortTransport_base(const DeviceInfo& info, const char* server,
			uint16_t port) : Transport(info), port_(port)
/* TODO: Low memory on extremely limited chips... only storing pointers for now
	{
		strncpy(server_, server, MAX_SERVER_LEN);
	}
*/
	, server_(server) { }
private:
	//char     server_[MAX_SERVER_LEN];
	const char* server_;
	uint16_t port_;
};


class MqttTransport_base : public ServerAndPortTransport_base {
protected:
	MqttTransport_base(const DeviceInfo& info, const char* server,
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

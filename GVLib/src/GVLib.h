/*
 * GreenVulcano Kit for Microcontrollers
 * Designed for AVR Chipsets (including Arduino)
 * Copyright (c) 2015 - GreenVulcano
 */

#ifndef GVLIB_H
#define GVLIB_H


#include <stdlib.h>
#include <stdint.h>


#define GV_DEVICES      "/devices"
#define GV_SENSORS      "/sensors"
#define GV_ACTUATORS    "/actuators"
#define GV_SENSOR_DATA  "/data"
#define GV_STATUS       "/status"

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


/*
 * On the Arduino Platform, the following two classes are provided by external
 * libraries, respectively to abstract Ethernet and MQTT access.
 * Keeping the same names allows the Arduino implementation to be small by
 * just including the related implementation files - on other targets, please
 * implement your own version.
 */
class Client;
class PubSubClient;


namespace gv {

class GVLib {
	public:
		GVLib();
		GVLib(const char* host, uint16_t port);
		GVLib(char server[], uint16_t port, const Client& ethClient);
		void setId(const char* id);
		void setName(const char* name);
		void setIp(const char* ip);
		void setPort(uint16_t port);
		void setServerPort(uint16_t port);
		void setServerHost(const char* host);
		void setDevice(const char* id, const char* name, const char* host, uint16_t port);

		// LAST WILL
		char* getStatusTopic();
		char* getStatusOffline();
		char* getStatusOnline();
		uint8_t getStatusQos();
		uint8_t getStatusRetain();

		// REST
		bool sendConfig(Client& client);
		bool sendSensorConfig(Client& client, uint8_t id, const char* name, const char* type);
		bool sendActuatorConfig(Client& client, uint8_t id, const char* name, const char* type, const char* topic);
		bool sendData(Client& client, uint8_t id, uint32_t value);

		// MQTT
		bool setClient(PubSubClient* client);
		bool sendConfig();
		bool sendSensorConfig(uint8_t id, const char* name, const char* type);
		bool sendActuatorConfig(uint8_t id, const char* name, const char* type, const char* topic);
		bool sendData(uint8_t id, uint32_t value);

	private:
		uint8_t _sensorCounter;

		char _buffer[BUFFER_SIZE];
		size_t _index;

		char _deviceId[DEVICE_ID_SIZE];
		char _deviceName[DEVICE_NAME_SIZE];
		char _deviceIp[HOST_SIZE];
		uint16_t _devicePort;

		char _host[HOST_SIZE]; //gv
		uint16_t _port; //gv

		char _will_topic[TOPIC_NAME_SIZE];
		char _will_message[32];

		PubSubClient* _client;

		// BUFFER
		char* getBuffer();
		void resetBuffer();
		size_t addToBuffer(const char* value, bool progmem_=false);
		size_t addToBuffer(uint16_t value);
		size_t addToBuffer(uint32_t value);
		size_t addToBuffer(int value);
		size_t addToBuffer(float value);
		bool send(Client& client, const char* service); // REST

		friend class GVLib_nonportable;
};


/**
	A class to hold callback delegates in GVLib.
*/
class Callback {
public:
	typedef void* (*CallbackPointer)(void*);

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
		@param payload the payload that shall be passed to the first callback
		               in the chain for the specific topic.
		@return the data returned by the last callback in the chain for the
		        specific topic.
	*/
	static void* call(const char* topic, void* payload);
	/**
		Performs clean-up and releases memory.
	*/
	static void dispose();

private:
	char             topic_[TOPIC_NAME_SIZE];
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


} // namespace gv
#endif GVLIB_H

/*
 * GreenVulcano Kit for Arduino
 * Copyright (c) 2015 - GreenVulcano
 */

#ifndef GVLIB_H
#define GVLIB_H

#include <Arduino.h>

#define GV_DEVICES      "/devices"
#define GV_SENSORS      "/sensors"
#define GV_ACTUATORS    "/actuators"
#define GV_SENSOR_DATA  "/data"
#define GV_STATUS       "/status"

#define NUMBER_ANALOG_PINS 6
#define NUMBER_DIGITAL_PINS 14
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

class Client;
class EthernetClient;
class PubSubClient;

namespace gv {


class Callback;

class GVLib {

	private:
		uint16_t 		_index;
		uint16_t 		_index_a; // index for actuator callback
		uint8_t			_sensorCounter;
		uint8_t			_actuatorCounter;
 		char 			_buffer[BUFFER_SIZE];

  		char			_deviceId[DEVICE_ID_SIZE];		
  		char 			_deviceName[DEVICE_NAME_SIZE];
  		char 			_deviceIp[HOST_SIZE];
  		uint16_t 		_devicePort;

  		char 			_host[HOST_SIZE]; //gv
  		uint16_t 		_port; //gv

  		char 			_will_topic[TOPIC_NAME_SIZE];
  		char 			_will_message[32];

  		PubSubClient* 	_client;

  		// BUFFER
  		char* getBuffer();
		void resetBuffer();
		void addToBuffer(char* value);
		void addToBuffer(String value);
		void addToBuffer(uint16_t value);
		void addToBuffer(uint32_t value);
		void addToBuffer(int value);
		void addToBuffer(float value);
		boolean send(Client& client, char* service); // REST
		void (*actuators_callback[MAX_ACTUATORS])(char*) = {};
		//void actuator_callback(char* topic, byte* payload, unsigned int length);
		//template <typename T> void sendBuffer(T& client, uint8_t chunkSize, uint8_t wait_time);

	public:
		GVLib();
		GVLib(char* host, uint16_t port);
		GVLib(byte server[], uint16_t port, EthernetClient ethClient);
		void setId(char* id);
		void setId(String id);
		void setName(char* name);
		void setName(String name);
		void setIp(char* ip);
		void setIp(String ip);
		void setPort(uint16_t port);
		void setServerPort(uint16_t port);
		void setServerHost(char* host);
		void setServerHost(String host);
		void setDevice(char* id, char* name, char* host, uint16_t port);

		// LAST WILL
		char* getStatusTopic();
		char* getStatusOffline();
		char* getStatusOnline();
		uint8_t getStatusQos();
		uint8_t getStatusRetain();

		// REST
		boolean sendConfig(Client& client);
		boolean sendSensorConfig(Client& client, uint8_t id, char* name, char* type);
		boolean sendActuatorConfig(Client& client, uint8_t id, char* name, char* type, char* topic);
		boolean sendData(Client& client, uint8_t id, uint32_t value);

		// MQTT
		boolean setClient(PubSubClient* client);
		boolean sendConfig();
		boolean sendSensorConfig(uint8_t id, char* name, char* type);
		boolean sendActuatorConfig(uint8_t id, char* name, char* type, char* topic);
		boolean sendData(uint8_t id, uint32_t value);

		int addActuator(uint8_t id, char* topic, void (*callback)(char*));
};


/**
	A class to hold callback delegates in GVLib.

	TODO: implement as a flat linked list: just one callback pointing to
	the next, and the selection of the right callback(/s) will be up to the
	`call` method.
*/
class Callback {
public:
	typedef void* (*FunctionPointer)(void*);
private:
	char*            topic;
	Callback*        next;
	FunctionPointer  function;

	static Callback* head;

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
	static Callback* add(const char* topic, FunctionPointer fn);

	/**
		Removes a callback from the global chain.
		Removes the `Callback` instance registered for the given topic and
		function pointer (or just for the given topic if no function pointer
		is specified).
		@param  topic the topic for which the callback shall be removed
		@param  fn    the function pointer to remove as a callback. Do not
		              pass it (or pass `0`) to remove ALL callbacks for a
					  given topic
		@return the number of callbacks found and removed from the chain.
	*/
	static int  remove(const char* topic, FunctionPointer fn=0);

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
};


} // namespace gv
#endif GVLIB_H

#ifndef GVLIB_H
#define GVLIB_H

#include "Arduino.h"
#include <Client.h>
#include <PubSubClient.h>
#include <Ethernet.h>

#define GV_DEVICES 		"/devices"
#define GV_SENSORS 		"/sensors"
#define GV_ACTUATORS 	"/actuators"
#define GV_SENSOR_DATA 	"/data"
#define GV_STATUS		"/status"

#define NUMBER_ANALOG_PINS 6
#define NUMBER_DIGITAL_PINS 14<
#define BUFFER_SIZE 160

#define DEVICE_ID_SIZE 10
#define DEVICE_NAME_SIZE 20
#define HOST_SIZE 20

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

  		char 			_will_topic[32];
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

#endif GVLIB_H
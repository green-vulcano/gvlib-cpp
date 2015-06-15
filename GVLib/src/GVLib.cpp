/*
 * GreenVulcano Kit for Arduino
 * Copyright (c) 2015 - GreenVulcano
 */

#include "GVLib.h"
#include "PubSubClient.h"

#include <Client.h>
#include <Ethernet.h>


namespace gv {

/**************************************************************************
 * 
 **************************************************************************/
GVLib::GVLib() {
	_index = 0;
	_sensorCounter = 0;
	_actuatorCounter = 0;
	resetBuffer();
}

/**************************************************************************
 * 
 **************************************************************************/
GVLib::GVLib(char* host, uint16_t port) {
	_index = 0;
	_sensorCounter = 0;
	_actuatorCounter = 0;
	resetBuffer();
	setServerHost(host);
	setServerPort(port);
}

/**************************************************************************
 * 
 **************************************************************************/
GVLib::GVLib(byte server[], uint16_t port, EthernetClient ethClient) {
	/*
	_index = 0;
	_sensorCounter = 0;
	_actuatorCounter = 0;
	resetBuffer();
	//setServerHost(host);
	//setServerPort(port);
	Serial.println("GVLIB INIT");

	byte mac[]    = { 0xDE, 0x1D, 0xBC, 0xEE, 0xFE, 0xED }; // mac shield arduino
	byte ip[]     = { 10, 100, 80, 28 }; // ip arduino

	EthernetClient ethClient2;
	Ethernet.begin(mac, ip);

	_client = PubSubClient(server, port, callback2, ethClient2);
	Serial.println("PubSubClient CREATED");

	bool c = false;
	while(c == false) {
    	Serial.println("connecting ...");
    
    	if(_client.connect("arduinoClient")) {
    		c = true;
    		Serial.println("CONNECTED");
    	}
    }

    _client.publish("/test", "OK FUNZIONA");

    _client.subscribe("gv/#");

    Serial.println("subscribed to 'gv/#'");
    */	
}

/**************************************************************************
 * 
 **************************************************************************/
void GVLib::setDevice(char* id, char* name, char* ip, uint16_t port) {
	setId(id);
	setName(name);
	setIp(ip);
	setPort(port);
}

/**************************************************************************
 * Set device id
 **************************************************************************/
void GVLib::setId(char* id) {
	strcpy(_deviceId, id);
}

/**************************************************************************
 * Set device id
 **************************************************************************/
void GVLib::setId(String id) {
	id.toCharArray(_deviceId, DEVICE_NAME_SIZE);
}

/**************************************************************************
 * Set device name
 **************************************************************************/
void GVLib::setName(char* name) {
	strcpy(_deviceName, name);
}

/**************************************************************************
 * Set device name
 **************************************************************************/
void GVLib::setName(String name) {
	name.toCharArray(_deviceName, DEVICE_NAME_SIZE);
}

/**************************************************************************
 * 
 **************************************************************************/
void GVLib::setIp(char* ip) {
	strcpy(_deviceIp, ip);
}

/**************************************************************************
 * 
 **************************************************************************/
void GVLib::setIp(String ip) {
	ip.toCharArray(_deviceIp, HOST_SIZE);
}

/**************************************************************************
 * 
 **************************************************************************/
void GVLib::setPort(uint16_t port) {
	_devicePort = port;
}

/**************************************************************************
 * 
 **************************************************************************/
void GVLib::setServerPort(uint16_t port) {
	_port = port;
}

/**************************************************************************
 * 
 **************************************************************************/
void GVLib::setServerHost(char* host) {
	strcpy(_host, host);
}

/**************************************************************************
 * 
 **************************************************************************/
void GVLib::setServerHost(String host) {
	host.toCharArray(_host, HOST_SIZE);
}

/**************************************************************************
 * 
 **************************************************************************/
void GVLib::addToBuffer(char* value) {
	uint16_t next = _index + strlen(value);

	for(int ii=0; ii<strlen(value); ii++) {
		_buffer[_index+ii] = value[ii];  
	}

	_index = _index + strlen(value);
}

/**************************************************************************
 * 
 **************************************************************************/
void GVLib::addToBuffer(String value) {
	for(int ii=0; ii<value.length(); ii++) {
		_buffer[_index+ii] = value[ii];  
	}

	_index = _index + value.length();
}

/**************************************************************************
 * 
 **************************************************************************/
void GVLib::addToBuffer(uint16_t value) {
	char number[10];
	itoa(value, number, 10);  
	addToBuffer(number);
}

/**************************************************************************
 * 
 **************************************************************************/
void GVLib::addToBuffer(uint32_t value) {
	char number[10];
	itoa(value, number, 10);  
	addToBuffer(number);
}

/**************************************************************************
 * 
 **************************************************************************/
void GVLib::addToBuffer(int value) {
	char number[10];
	itoa(value, number, 10);  
	addToBuffer(number);
}

/**************************************************************************
 * 
 **************************************************************************/
void GVLib::addToBuffer(float value) {
	char number[10];
	dtostrf(value, 5, 2, number);  
	addToBuffer(number);
}

/**************************************************************************
 * 
 **************************************************************************/
char* GVLib::getBuffer() {
 	return _buffer;
}

/**************************************************************************
 * 
 **************************************************************************/
void GVLib::resetBuffer() {
	_index = 0;
	memset(&_buffer[0], 0, sizeof(_buffer));
}

/**************************************************************************
 * 
 **************************************************************************/
boolean GVLib::setClient(PubSubClient* client) {
	this->_client = client;
}

/**************************************************************************
 * 
 **************************************************************************/
boolean GVLib::sendConfig(Client& client) {
	boolean result = false;

	resetBuffer();

	addToBuffer(F("{\"id\":\""));
	addToBuffer(_deviceId);
	addToBuffer(F("\",\"nm\":\""));
	addToBuffer(_deviceName);
	addToBuffer(F("\",\"ip\":\""));
	addToBuffer(_deviceIp);
	addToBuffer(F("\",\"prt\":"));
	addToBuffer(_devicePort);
	addToBuffer(F("}"));

	int num = 1;
	while(!result) {
		result = send(client, GV_DEVICES);
		#if DEBUG_MODE 
			Serial.print("attempt: ");
			Serial.println(num++);
		#endif
		delay(1000);
	}

	resetBuffer();

	return result;
}

/**************************************************************************
 * 
 **************************************************************************/
boolean GVLib::sendConfig() {
	resetBuffer();

	addToBuffer(getStatusOnline());
	_client->publish(getStatusTopic(), _buffer);

	resetBuffer();

	addToBuffer(F("{\"id\":\""));
	addToBuffer(_deviceId);
	addToBuffer(F("\",\"nm\":\""));
	addToBuffer(_deviceName);
	addToBuffer(F("\",\"ip\":\""));
	addToBuffer(_deviceIp);
	addToBuffer(F("\",\"prt\":"));
	addToBuffer(_devicePort);
	addToBuffer(F("}"));

	_client->publish(GV_DEVICES, _buffer);

	resetBuffer();

	return true;
}

/**************************************************************************
 * 
 **************************************************************************/
boolean GVLib::sendSensorConfig(Client& client, uint8_t id, char* name, char* type) {
	boolean response = false;

	resetBuffer();

	addToBuffer(F("{\"id\":"));
	addToBuffer(id);
	addToBuffer(F(",\"nm\":\""));
	addToBuffer(name);	
	addToBuffer(F("\",\"tp\":\""));
	addToBuffer(type);
	addToBuffer(F("\"}"));

	char srv[80];
	strcpy(srv, GV_DEVICES);
	strcat(srv, "/");
	strcat(srv, _deviceId);
	strcat(srv, GV_SENSORS);

	int num = 1;
	while(!response) {
		response = send(client, srv);

		if(num > MAX_ATTEMPTS) {
			return false;
		}
	}
	
	return response;
}

/**************************************************************************
 * 
 **************************************************************************/
boolean GVLib::sendSensorConfig(uint8_t id, char* name, char* type) {
	resetBuffer();

	addToBuffer(F("{\"id\":"));
	addToBuffer(id);
	addToBuffer(F(",\"nm\":\""));
	addToBuffer(name);	
	addToBuffer(F("\",\"tp\":\""));
	addToBuffer(type);
	addToBuffer(F("\"}"));

	char srv[80];
	strcpy(srv, GV_DEVICES);
	strcat(srv, "/");
	strcat(srv, _deviceId);
	strcat(srv, GV_SENSORS);

	_client->publish(srv, _buffer);

	resetBuffer();

	return true;
}

/**************************************************************************
 * 
 **************************************************************************/
boolean GVLib::sendActuatorConfig(Client& client, uint8_t id, char* name, char* type, char* topic) {
	boolean response = false;

	resetBuffer();

	addToBuffer(F("{\"id\":"));
	addToBuffer(id);
	addToBuffer(F(",\"nm\":\""));
	addToBuffer(name);	
	addToBuffer(F("\",\"tp\":\""));
	addToBuffer(type);
	addToBuffer(F("\",\"to\":\""));
	addToBuffer(topic);
	addToBuffer(F("\"}"));

	char srv[80];
	strcpy(srv, GV_DEVICES);
	strcat(srv, "/");
	strcat(srv, _deviceId);
	strcat(srv, GV_ACTUATORS);

	int num = 1;
	while(!response) {
		response = send(client, srv);

		if(num > MAX_ATTEMPTS) {
			return false;
		}
	}

	resetBuffer();

	return response;
}

//******************************************************************************
//
//******************************************************************************
boolean GVLib::sendActuatorConfig(uint8_t id, char* name, char* type, char* topic) {
	resetBuffer();

	addToBuffer(F("{\"id\":"));
	addToBuffer(id);
	addToBuffer(F(",\"nm\":\""));
	addToBuffer(name);	
	addToBuffer(F("\",\"tp\":\""));
	addToBuffer(type);
	addToBuffer(F("\",\"to\":\""));
	addToBuffer(topic);
	addToBuffer(F("\"}"));

	char srv[80];
	strcpy(srv, GV_DEVICES);
	strcat(srv, "/");
	strcat(srv, _deviceId);
	strcat(srv, GV_ACTUATORS);

	_client->publish(srv, _buffer);

	resetBuffer();

	return true;
}

//******************************************************************************
//
//******************************************************************************
boolean GVLib::sendData(Client& client, uint8_t id, uint32_t value) {
	boolean response = 0;
	char id_str[10];

	resetBuffer();

	sprintf(id_str,"%d", (int)id);

	addToBuffer(F("{\"value\":"));
	addToBuffer(value);
	addToBuffer(F("}"));

	char srv[80];
	strcpy(srv, GV_DEVICES);
	strcat(srv, "/");
	strcat(srv, _deviceId);
	strcat(srv, GV_SENSORS);
	strcat(srv, "/");
	strcat(srv, id_str);
	strcat(srv, "/");
	strcat(srv, GV_SENSOR_DATA);

	int num = 1;
	while(!response) {
		response = send(client, srv);

		if(num > MAX_ATTEMPTS) {
			return false;
		}
	}

	return response;
}

//******************************************************************************
//
//******************************************************************************
boolean GVLib::sendData(uint8_t id, uint32_t value) {
	char id_str[10];

	resetBuffer();

	sprintf(id_str,"%d", (int)id);

	addToBuffer(F("{\"id\":"));
	addToBuffer(id);
	addToBuffer(F(",\"value\":"));
	addToBuffer(value);
	addToBuffer(F("}"));

	char srv[80];
	strcpy(srv, GV_DEVICES);
	strcat(srv, "/");
	strcat(srv, _deviceId);
	strcat(srv, GV_SENSORS);
	strcat(srv, "/");
	strcat(srv, id_str);
	strcat(srv, "/");
	strcat(srv, GV_SENSOR_DATA);

	_client->publish(srv, _buffer);

	return true;
}

//******************************************************************************
//
//******************************************************************************
boolean GVLib::send(Client& client, char* service) {
	char outBuf[64];	
	int connectLoop = 0;
	boolean response = false;
	boolean connected = true;

	if(!client.connected()) {
		#if DEBUG_MODE 
			Serial.println("NOT CONNECTED");
		#endif
		connected = false;
		client.stop();

		if(client.connect(_host, _port) == 1) {
			#if DEBUG_MODE 
				Serial.println("NOW CONNECTED");
			#endif
			connected = true;
		}
	}

	if(connected) {
		#if DEBUG_MODE 
			Serial.println("TRYING SENDING DATA");
		#endif

		// HTTP POST
		sprintf(outBuf, "POST %s HTTP/1.1", service);
		client.println(outBuf);

		sprintf(outBuf, "Host: %s", _host);
		client.println(outBuf);
		client.println(F("Connection: close\r\nContent-Type: application/json; charset=utf-8"));

		sprintf(outBuf,"Content-Length: %u\r\n",strlen(_buffer));
		client.println(outBuf);

		// DATA		
   		client.println(_buffer);  

		// RESPONSE
		response = (boolean)client.read();
	}

  	return response;
}

//******************************************************************************
//
//******************************************************************************
int GVLib::addActuator(uint8_t id, char* topic, void (*callback)(char*)) {
//	int current_index = _index_a;
//	this->actuators_callback[current_index] = callback;
//	_index_a++;
	Serial.println("addActuator Called");

	// TODO

	//return current_index;

	return 0;
}

//******************************************************************************
//
//******************************************************************************
char* GVLib::getStatusTopic() {	
	strcpy(_will_topic, GV_DEVICES);
	strcat(_will_topic, "/");
	strcat(_will_topic, _deviceId);
	strcat(_will_topic, GV_STATUS);

	return _will_topic;
}

//******************************************************************************
//
//******************************************************************************
char* GVLib::getStatusOffline() {
	strcpy(_will_message, "{\"st\":false}");

	return _will_message;
}

//******************************************************************************
//
//******************************************************************************
char* GVLib::getStatusOnline() {
	strcpy(_will_message, "{\"st\":true}");

	return _will_message;
}

//******************************************************************************
//
//******************************************************************************
uint8_t GVLib::getStatusQos() {
	return 0;
}

//******************************************************************************
//
//******************************************************************************
uint8_t GVLib::getStatusRetain()  {
	return 0; // TODO: portare a 1
}

/**************************************************************************
 * 
 **************************************************************************/
 /*
template <typename T> 
void GVLib::sendBuffer(T& client, uint8_t chunkSize, uint8_t wait_time) {
	// Invio l'intero chunk
	if (chunkSize == 0) {
		client.print(_buffer);
	}
	else {  
		// numero massimo di iterazioni
		uint8_t max_iteration = (int)(_index/chunkSize)+1;

		// invio i dati
		for (uint8_t ii=0; ii< max_iteration; ii++) {
			char intermediate_buffer[chunkSize+1];

			memcpy(intermediate_buffer, _buffer + ii*chunkSize, chunkSize);
			intermediate_buffer[chunkSize] = '\0';

			client.print(intermediate_buffer);

			// attendo che il client riceva i dati
			delay(wait_time);
		}
	}

	resetBuffer();
}
*/

} //namespace gv
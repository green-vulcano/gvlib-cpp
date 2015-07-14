/*
 * GreenVulcano Kit for Arduino
 * Copyright (c) 2015 - GreenVulcano
 */

#include "GVLib.h"
#include "PubSubClient.h"

#include <Client.h>
#include <Ethernet.h>
#include <EthernetClient.h>

namespace gv {

	/**************************************************************************
	 * 
	 **************************************************************************/
	class GVLib_nonportable {
		public:
			static inline size_t addToBuffer(const __FlashStringHelper *str, GVLib *lib) {
				return lib->addToBuffer(reinterpret_cast<const char*>(str), true);
			}

			static void pubsub_callback(char *topic, uint8_t *payload, unsigned int ival) {			
		    	char *pBuffer = new char[ival+1];
		    	strncpy(pBuffer, (char*)payload, ival);
		    	pBuffer[ival] = '\0';
		    	gv::Callback::call(topic, pBuffer);
		    	delete[] pBuffer; 
			}
	};

	/**************************************************************************
	 * 
	 **************************************************************************/
	inline size_t addToBuffer(const __FlashStringHelper *str, GVLib *lib) {
		return GVLib_nonportable::addToBuffer(str, lib);
	}

	/**************************************************************************
	 * 
	 **************************************************************************/
	GVLib::GVLib() :
			_index(0),
			_sensorCounter(0),
			_devicePort(0),
			_port(0),
			_client(NULL)
	{

	}

	/**************************************************************************
	 * 
	 **************************************************************************/
	GVLib::GVLib(const char* host, uint16_t port) {
		setServerHost(host);
		setServerPort(port);
	}

	/**************************************************************************
	 * MQTT Constructor
	 **************************************************************************/
	GVLib::GVLib(char server[], uint16_t port, Client& ethClient, char id[], char name[]) {
		_index = 0;
		_sensorCounter = 0;
		resetBuffer();

		strcpy(_deviceId, id);
		strcpy(_deviceName, name);

		_client = new PubSubClient(server, port, GVLib_nonportable::pubsub_callback, ethClient);

		bool c = false;
		while(c == false) {
			#if DEBUG_MODE
	    		Serial.println("connecting ...");
	    	#endif

	    	resetBuffer();
			addToBuffer(GV_DEVICES);
			addToBuffer("/");
			addToBuffer(_deviceId);
			addToBuffer(GV_STATUS);

	    	if(_client->connect(_deviceId, _buffer, getStatusQos(), getStatusRetain(), GV_PAYLOAD_STATUS_OFFLINE)) {    	
	    		c = true;
	    		#if DEBUG_MODE
	    			Serial.println("CONNECTED");
	    		#endif
	    	}

	    	resetBuffer();
	    }
	}

	/**************************************************************************
	 * 
	 **************************************************************************/
	void GVLib::setDevice(const char* id, const char* name/*, const char* ip, uint16_t port*/) {
		setId(id);
		setName(name);
		// setIp(ip);
		// setPort(port);
	}

	/**************************************************************************
	 * Set device id
	 **************************************************************************/
	void GVLib::setId(const char* id) {
		strcpy(_deviceId, id);
	}

	/**************************************************************************
	 * Set device name
	 *************************************************************************/
	void GVLib::setName(const char* name) {
		strcpy(_deviceName, name);
	}

	/**************************************************************************
	 * 
	 **************************************************************************/
	void GVLib::setIp(const char* ip) {
		strcpy(_deviceIp, ip);
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
	void GVLib::setServerHost(const char* host) {
		strcpy(_host, host);
	}

	/**************************************************************************
	 * 
	 **************************************************************************/
	size_t GVLib::addToBuffer(const char* value, bool progmem_) {

		if (_index >= BUFFER_SIZE) {
			return 0;
		}

		char* (*cpy)(char*, const char*) = progmem_ ? strcpy_P : strcpy;
		char* (*ncpy)(char*, const char*, size_t) = progmem_ ? strncpy_P : strncpy;
		size_t (*len)(const char*) = progmem_ ? strlen_P : strlen;

		size_t length = len(value);
		size_t next = _index + length;

		if (next < BUFFER_SIZE) {
			cpy(_buffer+_index, value);
			_index = next;
		} else {
			length = BUFFER_SIZE - _index - 1;
			ncpy(_buffer+_index, value, length);
			_buffer[BUFFER_SIZE - 1] = '\0';
			_index = BUFFER_SIZE;
		}
		return length;
	}

	/**************************************************************************
	 * 
	 **************************************************************************/
	size_t GVLib::addToBuffer(uint16_t value) {
		char number[10];
		itoa(value, number, 10);  
		return addToBuffer(number);
	}

	/**************************************************************************
	 * 
	 **************************************************************************/
	size_t GVLib::addToBuffer(uint32_t value) {
		char number[10];
		itoa(value, number, 10);  
		return addToBuffer(number);
	}

	/**************************************************************************
	 * 
	 **************************************************************************/
	size_t GVLib::addToBuffer(int value) {
		char number[10];
		itoa(value, number, 10);  
		return addToBuffer(number);
	}

	/**************************************************************************
	 * 
	 **************************************************************************/
	size_t GVLib::addToBuffer(float value) {
		char number[10];
		dtostrf(value, 5, 2, number);  
		return addToBuffer(number);
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
	bool GVLib::setClient(PubSubClient* client) {
		this->_client = client;
	}

	/**************************************************************************
	 * REST
	 **************************************************************************/
	bool GVLib::sendConfig(Client& client) {
		bool result = false;

		resetBuffer();

		gv::addToBuffer(F("{\"id\":\""), this);
		addToBuffer(_deviceId);
		gv::addToBuffer(F("\",\"nm\":\""), this);
		addToBuffer(_deviceName);
		gv::addToBuffer(F("\",\"ip\":\""), this);
		addToBuffer(_deviceIp);
		gv::addToBuffer(F("\",\"prt\":"), this);
		addToBuffer(_devicePort);
		gv::addToBuffer(F("}"), this);

		int num = 1;
		while(!result) {
			result = send(client, GV_DEVICES);
			#if DEBUG_MODE 
				Serial.print(F("attempt: "));
				Serial.println(num++);
			#endif
			delay(1000);
		}

		resetBuffer();

		return result;
	}

	/**************************************************************************
	 * MQTT
	 **************************************************************************/
	bool GVLib::sendConfig() {
		resetBuffer();

		//addToBuffer(getStatusOnline());
		addToBuffer(GV_PAYLOAD_STATUS_ONLINE);
		_client->publish(getStatusTopic(), _buffer);

		resetBuffer();

		gv::addToBuffer(F("{\"id\":\""), this);
		addToBuffer(_deviceId);
		gv::addToBuffer(F("\",\"nm\":\""), this);
		addToBuffer(_deviceName);
		gv::addToBuffer(F("\""), this);
		//gv::addToBuffer(F("\",\"ip\":\""), this);
		//addToBuffer(_deviceIp);
		//gv::addToBuffer(F("\",\"prt\":"), this);
		//addToBuffer(_devicePort);
		gv::addToBuffer(F("}"), this);

		_client->publish(GV_DEVICES, _buffer);

		resetBuffer();

		return true;
	}

	/**************************************************************************
	 * REST
	 **************************************************************************/
	bool GVLib::sendSensorConfig(Client& client, uint8_t id, const char* name, const char* type) {
		bool response = false;

		resetBuffer();

		gv::addToBuffer(F("{\"id\":"), this);
		addToBuffer(id);
		gv::addToBuffer(F(",\"nm\":\""), this);
		addToBuffer(name);	
		gv::addToBuffer(F("\",\"tp\":\""), this);
		addToBuffer(type);
		gv::addToBuffer(F("\"}"), this);

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
	 * MQTT
	 **************************************************************************/
	bool GVLib::sendSensorConfig(uint8_t id, const char* name, const char* type) {
		resetBuffer();

		gv::addToBuffer(F("{\"id\":"), this);
		addToBuffer(id);
		gv::addToBuffer(F(",\"nm\":\""), this);
		addToBuffer(name);	
		gv::addToBuffer(F("\",\"tp\":\""), this);
		addToBuffer(type);
		gv::addToBuffer(F("\"}"), this);

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
	 * REST
	 **************************************************************************/
	bool GVLib::sendActuatorConfig(Client& client, uint8_t id, const char* name, const char* type, const char* topic) {
		bool response = false;

		resetBuffer();

		gv::addToBuffer(F("{\"id\":"), this);
		addToBuffer(id);
		gv::addToBuffer(F(",\"nm\":\""), this);
		addToBuffer(name);	
		gv::addToBuffer(F("\",\"tp\":\""), this);
		addToBuffer(type);
		gv::addToBuffer(F("\",\"to\":\""), this);
		addToBuffer(topic);
		gv::addToBuffer(F("\"}"), this);

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
	// MQTT
	//******************************************************************************
	bool GVLib::sendActuatorConfig(uint8_t id, const char* name, const char* type, const char* topic) {
		resetBuffer();

		gv::addToBuffer(F("{\"id\":"), this);
		addToBuffer(id);
		gv::addToBuffer(F(",\"nm\":\""), this);
		addToBuffer(name);	
		gv::addToBuffer(F("\",\"tp\":\""), this);
		addToBuffer(type);
		gv::addToBuffer(F("\",\"to\":\""), this);
		addToBuffer(topic);
		gv::addToBuffer(F("\"}"), this);

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
	// REST
	//******************************************************************************
	bool GVLib::sendData(Client& client, uint8_t id, uint32_t value) {
		bool response = 0;
		char id_str[10];

		resetBuffer();

		sprintf(id_str,"%d", (int)id);

		gv::addToBuffer(F("{\"value\":"), this);
		addToBuffer(value);
		gv::addToBuffer(F("}"), this);

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
	// MQTT
	//******************************************************************************
	bool GVLib::sendData(uint8_t id, uint32_t value) {
		char id_str[10];

		resetBuffer();

		sprintf(id_str,"%d", (int)id);

		gv::addToBuffer(F("{\"id\":"), this);
		addToBuffer(id);
		gv::addToBuffer(F(",\"value\":"), this);
		addToBuffer(value);
		gv::addToBuffer(F("}"), this);

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
	// REST
	//******************************************************************************
	bool GVLib::send(Client& client, const char* service) {
		char outBuf[64];	
		int connectLoop = 0;
		bool response = false;
		bool connected = true;

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
			response = (bool)client.read();
		}

	  	return response;
	}

	//******************************************************************************
	//
	//******************************************************************************
	void GVLib::regCallback(const char* topic, void* fn) {
		_client->subscribe((char*)topic);
		gv::Callback::add(topic, (gv::Callback::CallbackPointer)fn);
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

	//******************************************************************************
	//
	//******************************************************************************
	void GVLib::loop() {
		_client->loop();
	}

	/** 
	 * Class Callback --- definition 
	 */

	Callback* Callback::head_ = NULL;

	//******************************************************************************
	//
	//******************************************************************************
	Callback::Callback(const char* topic, CallbackPointer fn) : next_(NULL), function_(NULL) {
		strcpy(topic_, topic);
		function_ = fn;
	}

	//******************************************************************************
	//
	//******************************************************************************
	Callback* Callback::add(const char* topic, CallbackPointer fn) {
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
	int Callback::remove(const char* topic, CallbackPointer fn) {
		Callback *ptr = head_, *prev;
		while (find_(topic, fn, &ptr, &prev)) {
			Callback *bye = ptr;
			ptr = ptr->next_;
			if (prev) { // there is a 'previous' callback instance
				prev->next_ = ptr;
			} else {
				head_ = ptr;
			}
			delete bye;
		}
	}

	//******************************************************************************
	//
	//******************************************************************************
	void* Callback::call(const char* topic, void* payload) {
		Callback *ptr = head_, *prev;
		void* result = NULL;
		while (find_(topic, NULL, &ptr, &prev)) {
			if (ptr->function_) result = ptr->function_(payload);
			if (result) payload = result;
			ptr = ptr->next_;
		}
		return result;
	}

	//******************************************************************************
	//
	//******************************************************************************
	bool Callback::find_(const char* topic, CallbackPointer fn,
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

/*
 * GVTransports.cpp
 *
 *  Created on: Jul 15, 2015
 *      Author: Domenico Barra <eisenach@gmail.com>
 */

#include "gv/avr/arduino/transports.h"
#include "gv/avr/arduino/gv.h"

#include "PubSubClient.h"
#include "Client.h"

/**************************************************************************
 * 
 **************************************************************************/
bool gv::avr::arduino::RestTransport::connect() {
	int res;
	Serial.print(F("RestTransport: connecting... "));
	res = ethClient_.connect(server().v4(), port());

	if (res < 0) {
		Serial.println(F("FAILED!"));
		return false;
	}

	Serial.println(F("done."));
	return true;
}

/**************************************************************************
 * 
 **************************************************************************/
bool gv::avr::arduino::RestTransport::send(const char* service, size_t slen, const char* payload, size_t plen) {
	ensureConnected();
	char outBuf[64];

	// HTTP POST
	sprintf_P(outBuf, PSTR("POST %s HTTP/1.1"), service);
	ethClient_.println(outBuf);
	// TODO: this is IPv4 only for now - please upgrade!
	const uint8_t* ip = server().v4();
	sprintf_P(outBuf, PSTR("Host: %d.%d.%d.%d"), ip[0], ip[1], ip[2], ip[3]);
	ethClient_.println(outBuf);
	ethClient_.println(F("Connection: close\r\nContent-Type: application/json; charset=utf-8"));
	sprintf_P(outBuf,PSTR("Content-Length: %u\r\n"),strlen(payload));
	ethClient_.println(outBuf);
	ethClient_.println(payload); // DATA

	while (ethClient_.read() != -1) {
		// TODO: here we are throwing any incoming data away as we are not
		// providing any read operation. WE DO NOT WANT THIS on a long-term
		// basis (i.e. provide the response-related methods and keep the data)
	}

	return true;
}


/**************************************************************************
 * 
 **************************************************************************/
bool gv::avr::arduino::MqttTransport::connect() {
	bool res;
	Serial.println(F("MqttTransport: connecting... "));

	char willTopic[TOPIC_NAME_SIZE];
	sprintf_P(willTopic, PSTR("%s/%s%s"), GV_DEVICES, deviceInfo_.id(), GV_STATUS);

	if(username_ != NULL && password_ != NULL) {
			// AARGH!!! have to cast out of const because of mqtt lib!!
		res = mqttClient_.connect(
				const_cast<char*>(deviceInfo_.id()),			
				const_cast<char*>(username_),
				const_cast<char*>(password_),
				willTopic, 
				1, // QOS=1
				1, // RETAIN=1
				const_cast<char*>(GV_PAYLOAD_STATUS_OFFLINE));
	}
	else {
		res = mqttClient_.connect(
				const_cast<char*>(deviceInfo_.id()),			
				willTopic, 
				1, // QOS=1
				1, // RETAIN=1
				const_cast<char*>(GV_PAYLOAD_STATUS_OFFLINE));		
	}

	Serial.println(res ? F("Connection done.") :  F("Connection failed!"));

	if(res) {
		// status with RETAIN=1
		mqttClient_.publish(willTopic, (uint8_t*)GV_PAYLOAD_STATUS_ONLINE, sizeof(GV_PAYLOAD_STATUS_ONLINE)-1, 1);
	}

	return res;
}

/**************************************************************************
 * 
 **************************************************************************/
bool gv::avr::arduino::MqttTransport::send(const char* service, size_t slen, const char* payload, size_t plen) {
	if (!ensureConnected()) {
		return false;
	}
	
	// AARGH!!! have to cast out of const because of mqtt lib!!
	return mqttClient_.publish(const_cast<char*>(service), (uint8_t*)payload, plen);
}

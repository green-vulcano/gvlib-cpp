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


bool gv::avr::arduino::ArduinoTransport::sendSensorConfig(
		uint8_t id, const char* name, const char* type) {

	Buffer b;
	b.add(F("{\"id\":"));
	b.add(id);
	b.add(F(",\"nm\":\""));
	b.add(name);
	b.add(F("\",\"tp\":\""));
	b.add(type);
	b.add(F("\"}"));

	char srv[80];
	int srvlen = sprintf(srv, "%s/%s%s", GV_DEVICES, deviceInfo_.id(), GV_SENSORS);

	send(srv, srvlen, b.get(), b.len());
	return true;
}

bool gv::avr::arduino::ArduinoTransport::sendActuatorConfig(
		uint8_t id, const char* name, const char* type, const char* topic) {

	Buffer b;
	b.add(F("{\"id\":"));
	b.add(id);
	b.add(F(",\"nm\":\""));
	b.add(name);
	b.add(F("\",\"tp\":\""));
	b.add(type);
	b.add(F("\",\"to\":\""));
	b.add(topic);
	b.add(F("\"}"));

	char srv[80];
	int srvlen = sprintf(srv, "%s/%s%s", GV_DEVICES, deviceInfo_.id(), GV_ACTUATORS);

	send(srv, strlen(srv), b.get(), b.len());
	return true;

}
bool gv::avr::arduino::ArduinoTransport::sendData(uint8_t id, uint32_t value) {
	Buffer b;

	b.add(F("{\"id\":"));
	b.add(id);
	b.add(F(",\"value\":"));
	b.add((int)value);
	b.add(F("}"));

	char srv[80];
	int srvlen = sprintf(srv, "%s/%s%s/%d%s",
			GV_DEVICES, deviceInfo_.id(), GV_SENSORS, id, GV_SENSOR_DATA);

	send(srv, srvlen, b.get(), b.len());
	return true;

}



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

	// DATA
	ethClient_.println(payload);

	while (ethClient_.read() != -1) {
		// TODO: here we are throwing any incoming data away as we are not
		// providing any read operation. WE DO NOT WANT THIS on a long-term
		// basis (i.e. provide the response-related methods and keep the data)
	}

	return true;
}

bool gv::avr::arduino::RestTransport::sendConfig() {

	Buffer b;
	b.add(F("{\"id\":\""));
	b.add(deviceInfo_.id());
	b.add(F("\",\"nm\":\""));
	b.add(deviceInfo_.name());
	b.add(F("\",\"ip\":\""));
	b.add(deviceInfo_.ip());
	b.add(F("\",\"prt\":"));
	b.add((int)deviceInfo_.port());
	b.add(F("}"));

	const char* payload = b.get();
	int num = 1;
	bool result = false;
	while(!result) {
		result = send(GV_DEVICES, strlen(GV_DEVICES), payload, strlen(payload));
		#if DEBUG_MODE
			Serial.print(F("RestTransport: send attempt: "));
			Serial.println(num++);
		#endif
		delay(1000);
	}

	return result;

}


bool gv::avr::arduino::MqttTransport::connect() {
	bool res;
	Serial.print(F("MqttTransport: connecting... "));

	// AARGH!!! have to cast out of const because of implemented lib!!
	mqttClient_.connect(
			const_cast<char*>(deviceInfo_.id()),
			const_cast<char*>(statusTopic()),
			statusQos(), statusRetain(),
			const_cast<char*>(PSTR(GV_PAYLOAD_STATUS_OFFLINE)));

	Serial.println(res ? F("done.") :  F("FAILED!"));
	return res;
}

bool gv::avr::arduino::MqttTransport::send(const char* service, size_t slen, const char* payload, size_t plen) {
	return mqttClient_.publish((char *) service, (uint8_t *) payload, plen);
}

bool gv::avr::arduino::MqttTransport::sendConfig() {
	Buffer b;

	b.add(F(GV_PAYLOAD_STATUS_ONLINE));
	mqttClient_.publish(const_cast<char*>(statusTopic()), b.get());

	b.reset();
	b.add(F("{\"id\":\""));
	b.add(deviceInfo_.id());
	b.add(F("\",\"nm\":\""));
	b.add(deviceInfo_.name());
	b.add(F("\""));
	//b.add(F("\",\"ip\":\""));
	//b.add(deviceInfo_.ip());
	//b.add(F("\",\"prt\":"));
	//b.add(deviceInfo_.port());
	b.add(F("}"));

	mqttClient_.publish(const_cast<char*>(GV_DEVICES), b.get());

	return true;
}


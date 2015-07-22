/*
 * arduino/GVTransports.h
 *
 * Transport definitions for the Arduino Platform
 *
 *  Created on: Jul 15, 2015
 *      Author: Domenico Barra <eisenach@gmail.com>
 */

#ifndef GVTRANSPORTS_H_
#define GVTRANSPORTS_H_

#include "GVLib.h"
#include "Client.h"
#include "PubSubClient.h"

namespace gv {

namespace arduino {

class ArduinoTransport : public virtual Transport {
public:
	ArduinoTransport(const DeviceInfo& info) : Transport(info) { }

	bool sendSensorConfig(uint8_t id, const char* name,
			const char* type) override;
	bool sendActuatorConfig(uint8_t id, const char* name,
			const char* type, const char* topic) override;
	bool sendData(uint8_t id, uint32_t value) override;
};



class RestTransport : public ServerAndPortTransport_base, public ArduinoTransport {
public:
	RestTransport(const DeviceInfo& info, const IPAddr& server,uint16_t port,
			Client& ethClient) :
				Transport(info),
				ServerAndPortTransport_base(info, server, port),
				ArduinoTransport(info),
				ethClient_(ethClient) { }
	bool send(const char* service, size_t slen,
			const char* payload, size_t plen) override;
	bool sendConfig() override;
	bool connected()  override { return ethClient_.connected(); }
	bool disconnect() override { ethClient_.stop(); return true; };
	bool connect()    override;

	bool subscribe(const char* topic) override {
		// TODO implement subscription via REST
		return false;
	}
	bool poll() override {
		// TODO: implement polling via REST
		return false;
	}

private:
	Client&  ethClient_;

	RestTransport(const RestTransport&);
	RestTransport& operator=(const RestTransport&);
};


class MqttTransport : public MqttTransport_base, public ArduinoTransport {
public:
	MqttTransport(const DeviceInfo& info,
			const IPAddr& server, uint16_t port, Client& ethClient) :
				Transport(info),
				MqttTransport_base(info, server, port),
				ArduinoTransport(info),
				// TODO: IPv4-only for now, please upgrade!
				mqttClient_(const_cast<uint8_t*>(server.v4()), port,
							pubsub_callback, ethClient) { }

	bool send(const char* service, size_t slen,
			const char* payload, size_t plen) override;
	bool sendConfig() override;
	bool connected()  override { return mqttClient_.connected(); }
	bool disconnect() override { mqttClient_.disconnect(); return true; }
	bool connect()    override;

	bool subscribe(const char* topic) override {
		return mqttClient_.subscribe(const_cast<char*>(topic)); }
	bool poll() override { return mqttClient_.loop(); }
private:
	PubSubClient mqttClient_;

	MqttTransport(const MqttTransport&);
	MqttTransport& operator=(const MqttTransport&);

	static void pubsub_callback(char *topic, uint8_t *payload,
			unsigned int psize) {
		gv::GVComm::callback(topic, { payload, psize });
	}

};

}

}



#endif /* GVTRANSPORTS_H_ */

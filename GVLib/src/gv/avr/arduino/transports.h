/*
 * arduino/gv_transports.h
 *
 * Transport definitions for the Arduino Platform
 *
 *  Created on: Jul 15, 2015
 *      Author: Domenico Barra <eisenach@gmail.com>
 */

#ifndef GV_AVR_ARDUINO_TRANSPORTS_H_
#define GV_AVR_ARDUINO_TRANSPORTS_H_

#include "gv/gv.h"

#include "Client.h"
#include "PubSubClient.h"

namespace gv {
namespace avr {
namespace arduino {

/**************************************************************************
 * 
 **************************************************************************/
class RestTransport : public ServerAndPortTransport_base {
	public:
		RestTransport(const DeviceInfo& info, const IPAddr& server,uint16_t port, Client& ethClient) :
			ServerAndPortTransport_base(server, port), ethClient_(ethClient) { }

		bool send(const char* service, size_t slen, const char* payload, size_t plen) override;

		bool connected()  override { 
			return ethClient_.connected(); 
		}

		bool disconnect() override { 
			ethClient_.stop(); return true; 
		};

		bool connect() override;

		bool poll() override {
			// TODO: implement polling via REST
			return false;
		}

	private:
		Client&  ethClient_;
		RestTransport(const RestTransport&);
		RestTransport& operator=(const RestTransport&);
		bool handleSubscription(const char* topic, CallbackPointer fn) override {
			// TODO implement subscription via REST
			return false;
		}
};

/**************************************************************************
 * 
 **************************************************************************/
class MqttTransport : public ServerAndPortTransport_base {
	public:
		MqttTransport(const DeviceInfo& info, const IPAddr& server, uint16_t port, Client& ethClient) :
			// TODO: IPv4-only for now, please upgrade!
			ServerAndPortTransport_base(server, port), deviceInfo_(info), mqttClient_(const_cast<uint8_t*>(server.v4()), port, pubsub_callback, ethClient) { }

		bool send(const char* service, size_t slen, const char* payload, size_t plen) override;
		bool connect() override;

		bool connected() override { return mqttClient_.connected(); }
		bool disconnect() override { mqttClient_.disconnect(); return true; };

		bool poll() override { return mqttClient_.loop(); }

	private:
		PubSubClient mqttClient_;
		const DeviceInfo& deviceInfo_;
		MqttTransport(const MqttTransport&);
		MqttTransport& operator=(const MqttTransport&);

		bool handleSubscription(const char* topic, CallbackPointer fn) override {
			if (!ensureConnected()) {
				return false;
			}

			return mqttClient_.subscribe(const_cast<char*>(topic)); 
		}

		static void pubsub_callback(char *topic, uint8_t *payload, unsigned int psize) {
			payload[psize] = '\0'; // the library doesn't to it for us. so this is needed for safety.
			gv::GVComm::callback(topic, { payload, psize });
		}
};

}
}
}

#endif /* GV_AVR_ARDUINO_TRANSPORTS_H_ */

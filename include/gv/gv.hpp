/*****************************************************************************
 *              .:::::.                                                      *
 *             -+  .| :/                                                     *
 *             o ALAN  s                                                     *
 *             .+.`.| +-                __ _   _ __    ___    ___   _ __     *
 *               .:::y+ .....          / _` | | '__|  / _ \  / _ \ | '_ \    *
 *                   /yo++++o/-     . | (_| | | |    |  __/ |  __/ | | | |   *
 *   .::::.        -o/.       /s/+oo/  \__, | |_|     \___|  \___| |_| |_|   *
 *  o  @@ +-      .s-     .-.  -s:      ___ /      _                         *
 * :: @@ @ dyyyyyyso _-:::  `--.++     |___/      | |                        *
 * `o E@I o-      :s-          .s/    __   ___   _| | ___ __ _ _ __   ___    *
 *   .:::-         +o.        `oo`    \ \ / / | | | |/ __/ _` | '_ \ / _ \   *
 *                 .ss+:-..-:+o-       \ V /| |_| | | (_| (_| | | | | (_) |  *
 *           .:::./ho``.::::-`          \_/  \__,_|_|\___\__,_|_| |_|\___/   *
 *         .+ --- +.                                                         *
 *         o  GVT  o                          T e c h n o l o g i e s        *
 *         .+ --- +.                                                         *
 *           -:::-'                                                          * 
 *****************************************************************************
 *     GreenVulcano Communication Library for C++
 *     This file: gv/gv.hpp
 *                Main header file for basic GVLib operation.
 *****************************************************************************
 * Copyright (c) 2016, GreenVulcano Open Source Project. All rights reserved.
 *
 * This file is part of the GreenVulcano Communication Library for IoT.
 *
 * This is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * This software is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this software. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GV_HPP
#define GV_HPP

#include "gv_config.hpp"

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace gv {
using namespace std;

enum MessageType {
	REGISTER_DEVICE,
	REGISTER_SENSOR,
	REGISTER_ACTUATOR,
	COMMAND_ACTUATOR,
	STATUS,
	CONFIG,
	SENSOR_DATA,
	ALERT
};

class Message {
	string          src_id_;
	string          dst_id_;
	MessageType     type_;
	string          data_;
public:
	Message(const string& src_id, const string& dst_id, MessageType type, const string& data)
		: src_id_(src_id), dst_id_(dst_id), type_(type), data_(data) { }
	Message to_main_site(const string& src_id, MessageType type, const string& data) {
		return Message(src_id, "", type, data);
	}
	bool to_main_site() const { return dst_id_.empty(); }
	const string& src_id() const { return src_id_; }
	const string& dst_id() const { return dst_id_; }
	MessageType   type()   const { return type_;   }
	const string& data()   const { return data_;   }
};

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


/**************************************************************************
 * Class to support both IPv4 and IPv6 address families.
 **************************************************************************/
class IPAddr {
	public:		
		enum Type {
			IPv4, IPv6
		};

		IPAddr(const uint16_t data[]);
		IPAddr(const uint8_t data[], Type type=IPv4);

		const uint8_t* v4() const {
			return &(data_.ui8)[12]; 
		}

		const uint8_t* v6() const { 
			return data_.ui8; 
		}

		Type type() const {
			return ( memcmp(data_.ui8, IPv4_MASK, sizeof(IPv4_MASK)) != 0 ? IPv6 : IPv4 );
		}

		operator string() const;

	private:
		union {
			uint8_t  ui8[16];
			uint16_t ui16[8];
		} data_;

		static const uint8_t IPv4_MASK[12];
};

/**************************************************************************
 * 
 **************************************************************************/
class DeviceInfo {
	public:
		DeviceInfo(const string& id, const string& name, const IPAddr& addr, uint16_t port) : 
			id_(id), name_(name), addr_(addr), port_(port) { }

		const string&  id()   const { return id_;   }
		const string&  name() const { return name_; }
		const IPAddr&  ip()   const { return addr_; }
		uint16_t       port() const { return port_; }

	private:
		string   id_;
		string   name_;
		IPAddr   addr_;
		uint16_t port_;
};

/**************************************************************************
 * Callback parameter type
 **************************************************************************/ 
struct CallbackParam {
	void*   data;
	size_t  size;
	int     param;
};


/**************************************************************************
 * Callback function type.
 * By convention, if not returning *different* data, a callback
 * function must return the same callback param which it received
 * as input.
 **************************************************************************/
typedef CallbackParam (*CallbackPointer)(CallbackParam);

/**
 * Callback descriptor.
 * This structure carries a callback function pointer and an (optional)
 * integer parameter. Such parameter comes in handy if you use the same
 * callback for multiple topics, in order to distinguish them easily
 * without having to go through string comparison.
 *
 * Implicit conversion to and from `CallbackPointer` is provider for
 * backward compatibility and simplicity (no need to explicitly create
 * a `CallbackDescriptor` if the only thing you want is a `CallbackPointer`.
 */
struct CallbackDescriptor {
	CallbackPointer function;
	int             param;

	/**
	 * Constructor.
	 *
	 * @param  fn    the function to call back.
	 * @param  parm  an optional integer parameter that will be passed to the
	 *               callback function. If not passed, defaults to `0`
	 */
	CallbackDescriptor(CallbackPointer fn, int parm=0)
		: function(fn), param(parm) { }

	/**
	 * Implicit conversion to callback function pointer.
	 */
	operator CallbackPointer() const {
		return function;
	}
};


/**************************************************************************
 *	A class to hold callback delegates in GVLib.
 **************************************************************************/
class Callback {
	public:
		/**
			Adds a new callback to the global chain.
			Creates a new `Callback` instance, sets the corresponding values,
			and returns a pointer to it.
			@param  topic the topic on which this `Callback` shall listen.
			@param  desc  the callback descriptor.
			@return a pointer to the newly created instance (or 0 if the callback
			        was already found into the chain)
		*/
		static Callback* add(const string& topic, CallbackDescriptor desc);

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
		static int remove(const string& topic, CallbackPointer fn=NULL);

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
		static CallbackParam call(const string& topic, CallbackParam param);
		/**
			Performs clean-up and releases memory.
		*/
		static void dispose();

	private:
		string             topic_;
		Callback*          next_;
		CallbackDescriptor desc_;

		static Callback*   head_;

		Callback(const string& topic, CallbackDescriptor desc);

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
		static bool find_(const string& topic, CallbackPointer fn, Callback** ptr, Callback** prev);
};

/**************************************************************************
 *
 **************************************************************************/
class Transport {
	public:
		virtual bool connected()  = 0;
		virtual bool connect()    = 0;
		virtual bool disconnect() = 0;
		virtual bool send(const string& service, const string& data) = 0;

		bool subscribe(const string& topic, CallbackDescriptor cb, int param=0) {
			if (handleSubscription(topic, cb)) {
				GV_DEBUG("Subscribe: ");
				GV_DEBUGLN(topic);

				Callback::add(topic, cb);
				return true;
			}
			return false;
		}

		virtual bool poll() = 0;
		virtual ~Transport() = default;

	protected:
		Transport() { }
		virtual bool ensureConnected() { if (!connected()) { return connect(); } return true; }

	private:
		virtual bool handleSubscription(const string& topic, CallbackPointer cb) = 0;
		Transport(const Transport&);
		Transport& operator=(const Transport&);
};

/**************************************************************************
 *
 **************************************************************************/
class Protocol {
	public:
		Protocol(const DeviceInfo& info, Transport& transport) :
			transport_(transport), deviceInfo_(info) { }

		virtual bool addDevice(CallbackDescriptor desc=nullptr) = 0;
		virtual bool addSensor(const string& id, const string& name, const string& type) = 0;
		virtual bool addActuator(const string& id, const string& name, const string& type, CallbackDescriptor desc) = 0;
		virtual bool send(const Message& msg) = 0;

		virtual ~Protocol() = default;

	protected:
		Transport&         transport_;
		const DeviceInfo&  deviceInfo_;
};

/**************************************************************************
 *
 **************************************************************************/
class GVComm {
	public:
		GVComm(const DeviceInfo& deviceInfo, Transport& transport, Protocol& protocol);

		bool addCallback(const string& actuatorId, CallbackPointer fn, int param=0);

		bool addDevice(CallbackDescriptor desc=NULL);
		
		bool addSensor(const string& id, const string& name, const string& type) { 
			return protocol_.addSensor(id, name, type); 
		}

		bool addActuator(const string& id, const string& name, const string& type, CallbackDescriptor desc);

		bool send(const Message& msg) { 
			return protocol_.send(msg); 
		}

		bool poll();

		static CallbackParam callback(const string& topic, CallbackParam param) {
			return Callback::call(topic, param);
		}

	private:
		const DeviceInfo& deviceInfo_;
		Transport&        transport_;
		Protocol&         protocol_;
};

/**************************************************************************
 *
 **************************************************************************/
class ServerAndPort {
	protected:
		const IPAddr&  server() const { return server_; }
		uint16_t       port()   const { return port_; }

		ServerAndPort(const IPAddr& server, uint16_t port) : 
			server_(server), port_(port) { }		
	private:
		IPAddr   server_;
		uint16_t port_;
};

} // namespace gv

#endif // GV_GV_H

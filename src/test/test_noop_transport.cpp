#include "gv/gv.hpp"
#include "gv/protocol_v1.hpp"
#include "gv/noop_transport.hpp"

#include <string>
#include <iostream>
#include <cassert>

using namespace std;
using namespace gv;

string GLOBAL = "NONE";

CallbackParam func ( CallbackParam p ) {
	GLOBAL = p.data;
	return p;
}

int main() {
	DeviceInfo info { "deviceId", "DeviceName", "192.168.1.2", 1883 };
	trans::NoopTransport   trans;
	protocols::DefaultProtocol proto { info, trans };
	GVComm comm { info, trans, proto };

	trans.connect();

{
	comm.addDevice(nullptr);                     // this sends to /devices/deviceId
	trans.subscribe("/devices/deviceId", func);  // then we "low-level" subscribe
	trans.poll();                                // and wait for the echo!

	assert (GLOBAL == "{\"nm\":\"DeviceName\",\"ip\":\"192.168.1.2\",\"prt\":\"1883\"}");

	cout << "ADD_DEVICE : pass" << endl;
}

}
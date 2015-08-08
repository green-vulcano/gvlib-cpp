#include <gvlib_arduino.h>

#include <SPI.h>
#include <Ethernet.h>

using namespace gv;
using gv::CallbackParam;

const uint8_t myIp_[] = {10, 100, 80, 32};
const uint8_t serverIp_[] = {10, 100, 60, 103};
const int port = 1883;
const char device_id[] = "gv_028";
const char device_name[] = "GV Test Client";
byte mac[] = { 0xFA, 0x5F, 0x67, 0x5F, 0xBD, 0x85 }; 


/* * * * * * * * * * * * * * * * * * * * * * * * * * *
   GVLIB callback functions: they have to respect the
   prototype `CallbackParam function (CallbackParam p)`
   You need to register these functions in the
   `setup()` phase.
* * * * * * * * * * * * * * * * * * * * * * * * * * */
gv::CallbackParam callback1(gv::CallbackParam payload) {
  Serial.print(F("ARDUINO CALLBACK 1: "));
  Serial.println((char*)payload.data);
  return payload;
}

gv::CallbackParam callback2(gv::CallbackParam  payload) {
  Serial.print(F("ARDUINO CALLBACK 2: "));
  Serial.println((char*)payload.data);
  return payload;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * *
   GVLIB initialization: prefer static, so you 
   get little or no surprises (e.g. compared to when
   using "dynamic" memory.
* * * * * * * * * * * * * * * * * * * * * * * * * * */
IPAddr myIp (myIp_);
IPAddr serverIp (serverIp_);
DeviceInfo deviceInfo(device_id, device_name, myIp, port);
EthernetClient ethClient;

avr::arduino::MqttTransport mqttTransport(deviceInfo, serverIp, port, ethClient);
avr::DefaultProtocol protocol(deviceInfo, mqttTransport);
GVComm gvComm(deviceInfo, mqttTransport, protocol);


/* Arduino standard setup function */
void setup() {
  Serial.begin(115200);
  
  /* * * * * * * * * * * * * * *
     Enable ethernet first!
   * * * * * * * * * * * * * * */
  Ethernet.begin(mac, myIp.v4());
  
  /* * * * * * * * * * * * * * *
     Then you declare yourself
   * * * * * * * * * * * * * * */
  Serial.println(F("Sending Device Information: "));
  gvComm.sendDeviceInfo();

  /* * * * * * * * * * * * * * * * * *
     Then you declare your sensors...
   * * * * * * * * * * * * * * * * * */
  Serial.println(F("Sending Sensors Configuration: "));
  gvComm.sendSensorConfig(1, "Sensor TEST", "NUMERIC");  

  /* * * * * * * * * * * * * * * * * *
     ... and actuators
   * * * * * * * * * * * * * * * * * */
  Serial.println(F("Sending Actuators Configuration: "));
  gvComm.sendActuatorConfig(2, "Actuator TEST", "NUMERIC", "/topic/test");
  
  /* * * * * * * * * * * * * * * * * * * * * * *
     Don't forget to register your callbacks!
     (you can do this in the beginning too,
      but ALWAYS after Ethernet is initialized)
   * * * * * * * * * * * * * * * * * * * * * * */
  gvComm.addCallback("/test/callback/1", callback1);
  gvComm.addCallback("/test/callback/2", callback2);
}


/* Arduino standard loop function */
void loop() {
  Serial.println(F("LOOPING..."));
  gvComm.poll();
}

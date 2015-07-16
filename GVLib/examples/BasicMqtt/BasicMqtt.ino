#include <GVLib.h>
#include <arduino/GVArduino.h>

#include <SPI.h>
#include <Ethernet.h>


const char server[] = "10.100.60.103";    
const int port = 1883;

byte mac[] = { 0xFA, 0x5F, 0x67, 0x5F, 0xBD, 0x85 };
byte ip[]  = { 10, 100, 80, 32 };
const char ipstr[] = "10.100.80.32";
const char device_id[] = "gv_028";
const char device_name[] = "GV Test Client";

using namespace gv;
using gv::CallbackParam;

DeviceInfo deviceInfo(device_id, device_name, ipstr, port);
EthernetClient ethClient;
arduino::MqttTransport mqttTransport(deviceInfo, server, port, ethClient);
GVLib gvlib(deviceInfo, mqttTransport);

/*************************************************************
 * 
 *************************************************************/
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

gv::CallbackParam callback3(gv::CallbackParam  payload) {
  Serial.print(F("ARDUINO CALLBACK 3: "));
  Serial.println((char*)payload.data);
  return payload;
}

/*************************************************************
 * 
 *************************************************************/
void setup() {
  Serial.begin(115200);
  bool connection = false;
  Ethernet.begin(mac, ip);
  

  Serial.println(F("Sending Device Configuration: "));
  gvlib.sendConfig();

  Serial.println(F("Sending Sensors Configuration: "));
  gvlib.sendSensorConfig(1, "Sensor TEST", "NUMERIC");  

  Serial.println(F("Sending Actuators Configuration: "));
  gvlib.sendActuatorConfig(2, "Actuator TEST", "NUMERIC", "/topic/test");
  
  gvlib.addCallback("/test/callback/1", callback1);
  gvlib.addCallback("/test/callback/2", callback2);
  gvlib.addCallback("/test/callback/3", callback3);
}

/*************************************************************
 * 
 *************************************************************/
void loop() {
  gvlib.poll();
  Serial.println(F("LOOPING..."));
}


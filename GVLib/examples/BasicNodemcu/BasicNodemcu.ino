/*
 *  Simple HTTP get webclient test
 */     
#include <ESP8266WiFi.h>

#include <gvlib_arduino.h>
#include <ArduinoJson.h>

using namespace gv;
using gv::CallbackParam;
   
// Network and device configuration
const char* ssid     = "<INSERT_SSID_NAME>";
const char* password = "<INSERT_SSID_PASSWORD>";
const uint8_t myIp_[] = {10, 100, 50, 32};
const uint8_t serverIp_[] = {10, 100, 50, 27};
const int port = 1883;
const char device_id[] = "GVDEV_NODEMCU1";
const char device_name[] = "GV Device Test";
byte mac[] = { 0xFA, 0x5F, 0x67, 0x5F, 0xBD, 0x85 };

IPAddr myIp (myIp_);
IPAddr serverIp (serverIp_);
DeviceInfo deviceInfo(device_id, device_name, myIp, port);
WiFiClient wifiClient;

avr::arduino::MqttTransport mqttTransport(deviceInfo, serverIp, port, wifiClient);
avr::DefaultProtocol protocol(deviceInfo, mqttTransport);
GVComm gvComm(deviceInfo, mqttTransport, protocol);

/****************************************************
 * Callback for basic device operation
 ***************************************************/
gv::CallbackParam cbDevice(gv::CallbackParam payload) {
  StaticJsonBuffer<128> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject((const char*) payload.data);

  Serial.println("callback called");

  // Todo: callback action
}

/****************************************************
 * 
 ***************************************************/
void setup() {  
  Serial.begin(115200);
  delay(100);
 
  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  // Trying to connect
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

   // Connect the MQTT transport
   mqttTransport.connect();
   Serial.println(F("Sending Device Information: "));
   gvComm.addDevice(cbDevice);
}
 
/****************************************************
 * 
 ***************************************************/
void loop() {
  gvComm.poll();

  // Todo: main implementation

  delay(500);
}

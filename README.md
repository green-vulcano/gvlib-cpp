# gvlib-cpp
[GreenVulcano] (http://www.greenvulcano.com) communication library for Embedded Systems - C++ version.

Use this library to enable easy two-way communication between Embedded Systems (e.g. Arduino) and the GreenVulcano (E/I)OT Platform via MQTT and/or REST/HTTP


### Features
* Asynchronous message exchange
  * Send actively: you send a message by actively calling a `send(...)` method
  * Receive passively (but not so much): you subscribe to *topics*, associating one or more *callback* functions. When a payload is received onto the specified *topic*, as soon as you call `GVLib.poll()` it is passed as text to all the associated *callbacks*.
* Single management interface, multiple transports available
  * MQTT: two-way communication enabled for Arduino, now coming for all other AVR chips and different chipsets
  * REST: one-way (send-only) communication enabled for Arduino. Two-way communication coming, plus different
  chipset support.

### Examples
Have a look at the `examples` directory in order to get a quick idea on how to use the library.
We try (and are still working) to keep the interface as easy to understand as possible, please let us know
if you have suggestions.
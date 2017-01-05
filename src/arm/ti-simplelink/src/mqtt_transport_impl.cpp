#include "gv/mqtt_transport.hpp"
#include "mqtt_transport_impl.hpp"
#include "sl_mqtt_client.h" // Texas Instruments SmartLink MQTT Implementation

#include <memory>

namespace gv {
using ti_simplelink::PlatConfig;

class MqttDriver_sl : public MqttDriver {
    PlatConfig* config_;
public:
    MqttDriver_sl(void* plat_config)
        : config_(static_cast<PlatConfig*>(config)) { }
    int connect();
}

std::unique_ptr<MqttDriver> MqttTransport::create_pimpl_() {
    return std::make_unique<MqttDriver_sl>( static_cast<void*> plat_config_ );
}


int MqttDriver_sl::connect() {
    return 0;
} 


}
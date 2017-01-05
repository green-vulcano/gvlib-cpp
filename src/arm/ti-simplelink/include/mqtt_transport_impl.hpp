#ifndef GV_TI_SIMPLELINK_MQTT_TRANSPORT_IMPL_HPP
#define GV_TI_SIMPLELINK_MQTT_TRANSPORT_IMPL_HPP

#include <string>
#include <cstdint>

namespace gv {
    namespace ti_simplelink {

        struct PlatConfig {
            std::string    broker_address;
            std::uint16_t  broker_port;
            std::uint8_t   mqtt_task_priority;
        };

    }
}


#endif
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
 * Copyright (c) 2015-2017, GreenVulcano Open Source Project.
 * All rights reserved.
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
 *****************************************************************************
 *     GreenVulcano Communication Library for C++
 *     This file  : ti-simplelink/include/mqtt_transport_impl.cpp
 *                  MQTT Transport for TI-SmartLink - implementation.
 *     Created on : Jan 4, 2017.
 *     Author     : Domenico Barra <eisenach@gmail.com>
 ****************************************************************************/


#include "simplelink_wrapper.hpp"

#include "gv/mqtt_transport.hpp"
#include "mqtt_transport_impl.hpp"

#include <memory>
#include <cstdarg>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cerrno>


// Texas Instruments bare-metal and SimpleLink includes
#include "hw_types.h"
#include "hw_memmap.h"      // Where in RAM are the hardware ports mapped
#include "rom_map.h"        // Uses ROM if available, defaults to RAM
#include "uart.h"           // UART low-level interface
#include "sl_mqtt_client.h" // SimpleLink MQTT Client implementation



namespace gv { namespace ti_simplelink { namespace trans { namespace mqtt {

using Driver = gv::trans::mqtt::Driver;
using Transport = gv::trans::mqtt::Transport;
using WillConfig = gv::trans::mqtt::WillConfig;


namespace {
    using namespace std;

    static unsigned long UART_BASE = UARTA0_BASE;

    void sl_uart_puts(const char *str)
    {
    #ifndef NOTERM
        if(str != NULL)
        {
            while(*str!='\0')
            {
                MAP_UARTCharPut(UART_BASE,*str++);
            }
        }
    #endif
    }
    
    int sl_uart_printf(const char *fmt, ...)
    {
        int iRet = 0;
        #ifndef NOTERM

        char *pcBuff, *pcTemp;
        int iSize = 256;
        
        va_list list;
        pcBuff = (char*)malloc(iSize);
        if(pcBuff == NULL)
        {
            return -1;
        }
        while(1)
        {
            va_start(list,fmt);
            iRet = vsnprintf(pcBuff,iSize,fmt,list);
            va_end(list);
            if(iRet > -1 && iRet < iSize)
            {
                break;
            }
            else
            {
                iSize*=2;
                if((pcTemp=(char*)realloc(pcBuff,iSize))==NULL)
                { 
                    sl_uart_puts("Could not reallocate memory\n\r");
                    iRet = -1;
                    break;
                }
                else
                {
                    pcBuff=pcTemp;
                }
                
            }
        }
        sl_uart_puts(pcBuff);
        free(pcBuff);
        
        #endif
        return iRet;
    }

    void gv_to_sl(const PlatConfig& src, SlMqttClientLibCfg_t& dst) {
        dst.loopback_port   = 0;
        dst.rx_tsk_priority = src.mqtt_task_priority;
        dst.aux_debug_en    = src.debug;

        // the following cast is ugly but necessary since the printf-like
        // function pointer defined in SlMqttClientLibCfg_t declares the
        // return value as long - while the *printf family declares it
        // as int.
        dst.dbg_print = (long(*)(const char *, ...))
                        (src.dbg_print ? src.dbg_print : sl_uart_printf);
    }

    void gv_to_sl(const WillConfig& src, SlMqttWill_t& dst) {
        dst.will_topic = src.will_topic.c_str();
        dst.will_msg = src.will_message.c_str();
        dst.will_qos = static_cast<char>(src.will_qos);
        dst.retain   = src.will_retain;
    }

    void gv_to_sl(const Transport& trs, SlMqttClientCtxCfg_t& dst) {       
//        dst.server_info.netconn_flags = 0x0; //SL_MQTT SL_MQTT_NETCONN_URL;
        memset(&dst, '\0', sizeof(dst));
        dst.server_info.server_addr = trs.server().c_str();
        dst.server_info.port_number = trs.port();
        dst.mqtt_mode31 = false;
        dst.blocking_send = true;
    }

/* Callback functions for the SimpleLink MQTT Client */
extern "C" {

void
sl_cb_Mqtt_Recv(void *app_hndl, const char  *topstr, long top_len, const void *payload,
                       long pay_len, bool dup, unsigned char qos, bool retain)
{
    Callback::call(
        string(topstr, top_len),
        gv::CallbackParam( string(static_cast<const char*>(payload), pay_len)) );
}




void sl_cb_MqttEvt(
    void *app_hndl, long evt, const void *buf, unsigned long len)
{
    switch(evt)
    {
      case SL_MQTT_CL_EVT_PUBACK:
        break;
    
      case SL_MQTT_CL_EVT_SUBACK:
        break;
        
      case SL_MQTT_CL_EVT_UNSUBACK:
        break;
    
      default:
        break;
  
    }
}

void sl_cb_MqttDisconnect(void *app_hndl)
{
    sl_uart_puts("mqtt disconnect() called\n\r");
}

} // extern "C"



} // anonymous namespace

void set_uart_base(unsigned long base) {
    UART_BASE = base;
}


class MqttDriver_sl : public Driver {
    Transport*  transport_;
    PlatConfig* config_;
    WillConfig* will_config_;
    void*       clt_ctx_;
    bool        connected_;
public:
    MqttDriver_sl(Transport* transport, void* plat_config, WillConfig* will_config)
        : transport_(transport),
          config_(static_cast<PlatConfig*>(plat_config)),
          will_config_(will_config), connected_(false) { }
    Status connect() override;
    bool connected() override { return connected_; }
    Status disconnect() override;
	Status send(const std::string& service, const std::string& payload) override;
	StatusRet<bool> poll() override;
    Status handleSubscription(const string& topic, CallbackPointer fn) override;
};




Status MqttDriver_sl::connect() {
    if (connected_) return Status::ok();

    config_->dbg("MqttDriver_sl: connecting\n\r");

    SlMqttClientLibCfg_t  sl_cfg;
    SlMqttClientCtxCfg_t  sl_clt_ctx_cfg_;
    SlMqttClientCbs_t     sl_cbs = { sl_cb_Mqtt_Recv, sl_cb_MqttEvt, sl_cb_MqttDisconnect };

    gv_to_sl(*config_, sl_cfg);
    gv_to_sl(*transport_, sl_clt_ctx_cfg_);

    _i32 res = sl_ExtLib_MqttClientInit(&sl_cfg);
    if (res != 0) {
        return Status::by_code(-ECONNABORTED);
    }
    // Creating the Client Context, passing the current sl_MqttDriver instance
    // as app handle.
    
    clt_ctx_ = sl_ExtLib_MqttClientCtxCreate(&sl_clt_ctx_cfg_, &sl_cbs, this);
    const string& device_id = transport_->device_info().id();
    sl_ExtLib_MqttClientSet(clt_ctx_, SL_MQTT_PARAM_CLIENT_ID, device_id.c_str(), device_id.length());


    if (!will_config_->will_topic.empty() && !will_config_->will_message.empty()) {
        SlMqttWill_t sl_will;
        gv_to_sl(*will_config_, sl_will);
        sl_ExtLib_MqttClientSet(clt_ctx_, SL_MQTT_PARAM_WILL_PARAM, &sl_will, sizeof(SlMqttWill_t));
    }

    if (!transport_->username().empty()) {
        sl_ExtLib_MqttClientSet(clt_ctx_, SL_MQTT_PARAM_USER_NAME,
            transport_->username().c_str(), transport_->username().length());
        if (!transport_->password().empty()) {
            sl_ExtLib_MqttClientSet(clt_ctx_, SL_MQTT_PARAM_PASS_WORD,
                transport_->password().c_str(), transport_->password().length());
        }
    }

    res = sl_ExtLib_MqttClientConnect(clt_ctx_, true, config_->keep_alive_secs);
    if ((res & 0xFF) != 0) {
        config_->dbg("MqttDriver_sl: connection aborted.");
        return Status::by_code(-ECONNABORTED);
    }
    config_->dbg("MqttDriver_sl: connection established.");
    
    connected_ = true;
    return Status::ok(); 
}

Status MqttDriver_sl::disconnect() {
    if (!connected_) return Status::ok();
    _i32 res = sl_ExtLib_MqttClientDisconnect(clt_ctx_);
    if (res == 0) {
        connected_ = false;
        sl_ExtLib_MqttClientCtxDelete(clt_ctx_);
        sl_ExtLib_MqttClientExit();
        clt_ctx_ = nullptr;
        return Status::ok();
    }
    return Status::unknown(); // currently don't know why disconnect failed
                              // TODO: improve diagnostics here
}

Status MqttDriver_sl::send(const std::string& service, const std::string& payload) {
    if (!connected_) return Status::by_code(-ENOTCONN);

    _i32 res = sl_ExtLib_MqttClientSend(clt_ctx_, service.c_str(),
          payload.c_str(), payload.length(),  config_->default_qos, false); // TODO: retain not handled for now
    if (res == 0) {
        return Status::ok();
    }
    return Status::unknown(); // TODO: better return code handling / translation
}

Status MqttDriver_sl::handleSubscription(const string& topic, CallbackPointer fn) {
    char* topics[] { const_cast<char*>(topic.c_str()) };
    _u8   qos[]    { config_->default_qos };
    _i32 res = sl_ExtLib_MqttClientSub(clt_ctx_, topics, qos, 1);
    if (res == 0) {
        Callback::add(topic, fn);
        return Status::ok();
    }
    return Status::unknown(); // TODO: better return code handling / translation    
}

StatusRet<bool> MqttDriver_sl::poll()  {
    // currently only asynchronous
    return StatusRet<bool>::ok(false);
}

}}}} // namespace gv::ti_simplelink::trans:mqtt


std::unique_ptr<gv::trans::mqtt::Driver> gv::trans::mqtt::Transport::create_driver_() {
    return std::make_unique<gv::ti_simplelink::trans::mqtt::MqttDriver_sl>(
        this, static_cast<void*>(plat_config_), &will_config_ );
}
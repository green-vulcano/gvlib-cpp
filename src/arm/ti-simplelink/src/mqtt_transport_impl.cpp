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


/*
   CAUTION!!!
     A bug in TI's simplelink.h header (at least in version 1.0.1.6) causes an
     extern "C" declaration to be opened once and closed twice within that
     file. Here we work around this problem by using nesting, so we open an
     extern "C" before including "simplelink.h", and then test the specific
     SimpleLink version to check if we need to close it.
   TODO: keep track of future SimpleLink versions
 */ 
extern "C" {
#include "simplelink.h"
#if (! (SL_MAJOR_VERSION_NUM==1L && SL_MINOR_VERSION_NUM==0L && SL_VERSION_NUM==1L && SL_SUB_VERSION_NUM==6L ))
    } // close extern "C"
#endif

#include "gv/mqtt_transport.hpp"
#include "mqtt_transport_impl.hpp"

#include <memory>
#include <cstdarg>
#include <cstdlib>
#include <cstdio>


// Texas Instruments bare-metal and SimpleLink includes
#include "hw_types.h"
#include "hw_memmap.h"      // Where in RAM are the hardware ports mapped
#include "rom_map.h"        // Uses ROM if available, defaults to RAM
#include "uart.h"           // UART low-level interface
#include "sl_mqtt_client.h" // SimpleLink MQTT Client Implementation



namespace gv {
using PlatConfig = ti_simplelink::SlMqtt_PlatConfig;



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


} // anonymous namespace

void ti_simplelink::set_uart_base(unsigned long base) {
    UART_BASE = base;
}


class MqttDriver_sl : public MqttDriver {
    PlatConfig* config_;
public:
    MqttDriver_sl(void* plat_config)
        : config_(static_cast<PlatConfig*>(plat_config)) { }
};

std::unique_ptr<MqttDriver> MqttTransport::create_driver_() {
    return std::make_unique<MqttDriver_sl>( static_cast<void*>(plat_config_) );
}



}
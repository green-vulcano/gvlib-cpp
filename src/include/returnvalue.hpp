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
 *     GreenVulcano C++ Libraies
 *     This file: returnvalue.hpp
 *                Generic return-value type
 *****************************************************************************
 * Copyright (c) 2016, GreenVulcano Open Source Project. All rights reserved.
 *
 * This file is part of the GreenVulcano Integration Platform.
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

#ifndef GV_RETURNVALUE_HPP
#define GV_RETURNVALUE_HPP

#include <utility>
#include <map>
#include <string>

namespace gv {


class Status {
    int                               code_;
    std::string                       description_;
    static std::map<int, Status>&     by_code_map() {
        static std::map<int, Status>      BY_CODE;
        return BY_CODE;
    }
public:
    Status() : code_(ok().code_), description_(ok().description_) { }
    Status(int code, const std::string& description) : code_(code), description_(description) { }
    int code() const { return code_; }
    const std::string& description() const { return description_; }
    bool is_ok() const { return code_ == 0; }
    bool is_system_error() const { return code_ < 0; }
    bool is_business_error() const { return code_ > 0; }

    static Status* by_code(int code) {
        auto it = by_code_map().find(code);
        if (it != by_code_map().end()) return &it->second;
        return nullptr;
    }
    static const Status& ok() {
        static const Status OK(0, "OK");
        return OK;
    }

    static void record(int code, const std::string& description) {
        by_code_map()[code] = Status(code, description);
    }
};



template <typename R> class ReturnValue {
    Status sts_;
    R      ret_;
public:
    ReturnValue(const Status& sts, R&& ret): sts_(sts), ret_(ret) { }
    static ReturnValue ok(R&& ret) { return ReturnValue(Status::ok(), std::move(ret)); }
    static ReturnValue by_status_code(int code, R&& ret) {
        return ReturnValue(Status::by_code(code), std::move(ret));
    }
    R value() const { return ret_; }
    Status status() const { return sts_; }
};


}

#endif
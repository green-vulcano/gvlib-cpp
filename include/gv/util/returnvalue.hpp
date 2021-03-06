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
 *     GreenVulcano C++ Libraries
 *     This file: gv/util/returnvalue.hpp
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

#ifndef GV_UTIL_RETURNVALUE_HPP
#define GV_UTIL_RETURNVALUE_HPP

#include <utility>
#include <map>
#include <string>
#include <limits>
#include <cstring>

namespace gv { namespace util {

/**
 * Represents the return status of a function (i.e. OK or error).
 */
class Status {
    int                               code_;
    std::string                       description_;
    static std::map<int, Status>&     by_code_map() {
        static std::map<int, Status>      BY_CODE;
        return BY_CODE;
    }
public:
    /**
     * Default constructor, constructs as Status::ok()
     */
    Status() : code_(ok().code_), description_(ok().description_) { }
    /**
     * Constructs with a custom code and description -
     * i.e. not recording into the code map
     */
    Status(int code, const std::string& description) :
        code_(code), description_(description) { }
    Status(Status&& s)               = default;
    Status(const Status& s)          = default;
    Status& operator=(const Status&) = default;
    Status& operator=(Status&&)      = default;

    int code() const { return code_; }
    const std::string& description() const { return description_; }
    bool is_ok() const { return code_ == 0; }
    bool is_system_error() const { return code_ < 0; }
    bool is_business_error() const { return code_ > 0; }

    operator int() const { return code_; }

    static const Status& by_code(int code) {
        auto it = by_code_map().find(code);
        if (it != by_code_map().end()) return it->second;
        record(code, std::strerror(code < 0 ? -code : code));
        return by_code(code);
    }
    static const Status& ok() {
        static const Status OK_(0, "OK");
        return OK_;
    }
    static const Status& unknown() {
        static const Status UNKNOWN_(std::numeric_limits<int>::max(), "UNKNOWN");
        return UNKNOWN_;
    }

    static void record(int code, const std::string& description) {
        by_code_map()[code] = Status(code, description);
    }

};


/**
 *
 */
template <typename R> class StatusRet {
    Status sts_;
    char   storage_[sizeof(R)];
    bool   value_set_;
public:
    StatusRet(const Status& sts, R&& ret): sts_(sts), value_set_(true) {
        R* handle = reinterpret_cast<R*>(storage_);
        new(handle) R(std::move(ret));
    }
    StatusRet(const Status& sts, const R& ret): sts_(sts), value_set_(true) {
        R* handle = reinterpret_cast<R*>(storage_);
        new(handle) R(ret);
    }
    StatusRet(const Status& sts): sts_(sts), value_set_(false) { }
    StatusRet(StatusRet&& mv) = default;

    static StatusRet ok(R&& ret) { return StatusRet(Status::ok(), std::move(ret)); }
    static StatusRet ok(const R& ret) { return StatusRet(Status::ok(), ret); }
    static StatusRet by_status_code(int code, R&& ret) {
        return StatusRet(Status::by_code(code), std::move(ret));
    }
    static StatusRet by_status_code(int code, const R& ret) {
        return StatusRet(Status::by_code(code), ret);
    }
    bool is_value_set() const { return value_set_; }
    const R value() const { return *(reinterpret_cast<const R*>(storage_)); }
    Status status() const { return sts_; }
 
    operator Status() const { return status(); }
    operator R() const { return value(); }

private:
    StatusRet(const StatusRet&)            = delete;
    StatusRet& operator=(const StatusRet&) = delete;
};


}} // namespace gv::util

#endif
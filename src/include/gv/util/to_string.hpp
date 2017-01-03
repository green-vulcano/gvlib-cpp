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
 *     This file: gv/util/to_string.hpp
 *                Convenience to_string abstraction
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

#ifndef GV_UTIL_TO_STRING_HPP
#define GV_UTIL_TO_STRING_HPP

#include <string>

#if defined(__GNUC__) && (__cplusplus < 201103L || !defined(_GLIBCXX_USE_C99))
/*
   We have to define our own version of "to_string" since most probably we are
   using newlib, which has (for now) incomplete support for C99 (long double
   functionality is missing) so the GNU C++ library disables the to_string
   set of functions altogether. 
 */
    #define GV_NEED_OWN_TO_STRING
    #include <cstdio> 
    #include <limits>
#endif


namespace gv { namespace util { 

using namespace std;

#if defined(GV_NEED_OWN_TO_STRING)

  template <typename T, int bufsize=4*sizeof(T)> string own_to_string(const char* fmt, T val)
  {
      char buf[bufsize];
      snprintf(buf, bufsize, fmt, val);
      return string(buf);
  }

  inline string to_string(int val)
  { return own_to_string("%d", val); }

  inline string to_string(unsigned val)
  { return own_to_string("%u", val); }

  inline string to_string(long val)
  { return own_to_string("%ld", val); }

  inline string to_string(unsigned long val)
  { return own_to_string("%lu", val); }

  inline string to_string(long long val)
  { return own_to_string("%lld", val); }

  inline string to_string(unsigned long long val)
  { return own_to_string("%llu", val); }

  inline string to_string(float val)
  {
    const int bufsize = numeric_limits<float>::max_exponent10 + 20;
    return own_to_string<float, bufsize>("%f", val);
  }

  inline string to_string(double val)
  {
    const int bufsize = numeric_limits<double>::max_exponent10 + 20;
    return own_to_string<double, bufsize>("%f", val);
  }

  inline string to_string(long double val)
  {
    const int bufsize = numeric_limits<long double>::max_exponent10 + 20;
    return own_to_string<long double, bufsize>("%Lf", val);
  }


#else
      using std::to_string;
#endif

inline const std::string& to_string(const std::string& str) { return str; }
inline       std::string& to_string(      std::string& str) { return str; }
}}

#endif
/*
Userspace Virtual Filesystem

Copyright (C) 2015 Sebastian Herbord. All rights reserved.

This file is part of usvfs.

usvfs is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

usvfs is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with usvfs. If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once
#include "usvfs_shared/logging.h"

#include <stdexcept>

namespace MyBoostFake {
struct error_base {};

template <typename TagT, typename ValueT>
struct error_info : error_base {
    typedef ValueT value_type;
    error_info(const ValueT&) {}
};

class exception : public std::exception {};

template <class ExceptionT, class TagT, typename ValueT>
const ExceptionT& operator<<(const ExceptionT& ex, const error_info<TagT, ValueT>&) {
    return ex;
}

template <class InfoT, class ExceptionT>
typename InfoT::value_type* get_error_info(const ExceptionT&) {
    static InfoT::value_type def;
    return &def;
}

// FIXME: This?
template <typename Exec, typename T>
void debug_throw(const Exec& e, std::string, T) {
    throw e;
}

} // namespace MyBoostFake

namespace MyBoost = MyBoostFake;

typedef MyBoost::error_info<struct tag_message, unsigned long> ex_win_errcode;
typedef MyBoost::error_info<struct tag_message, std::string> ex_msg;

struct incompatibility_error : MyBoost::exception {};
struct usage_error : MyBoost::exception {};
struct data_error : MyBoost::exception {};
struct file_not_found_error : MyBoost::exception {};
struct timeout_error : MyBoost::exception {};
struct unknown_error : MyBoost::exception {};
struct node_missing_error : MyBoost::exception {};

#define USVFS_S1(x) #x
#define USVFS_S2(x) USVFS_S1(x)
#define USVFS_THROW_EXCEPTION(x) ::MyBoost::debug_throw((x), (USVFS_S2(__FILE__)), (USVFS_S2(__LINE__)))

void logExtInfo(const std::exception& e, LogLevel logLevel = LogLevel::Warning);

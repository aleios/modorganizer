/*
Copyright (C) 2012 Sebastian Herbord. All rights reserved.

This file is part of Mod Organizer.

Mod Organizer is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Mod Organizer is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Mod Organizer.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

#include <common/sane_windows.h>
#include <string>

namespace std {
#ifdef UNICODE
typedef wstring tstring;
#else
typedef string tstring;
#endif
} // namespace std

extern void log(const char* format, ...);

namespace MOShared {

void reportError(LPCSTR format, ...);
void reportError(LPCWSTR format, ...);

std::string getCurrentErrorStringA();
std::wstring getCurrentErrorStringW();

#ifdef UNICODE
#define getCurrentErrorString getCurrentErrorStringW
#else
#define getCurrentErrorString getCurrentErrorStringA
#endif

} // namespace MOShared

/*
U2FDevice - A program to allow Raspberry Pi Zeros to act as U2F tokens
Copyright (C) 2018  Michael Kuc

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#define ARCH_RASPBERRY_PI 1
#define ARCH_ANDROID 2
#define ARCHITECTURE ARCH_ANDROID

#if ARCHITECTURE == ARCH_RASPBERRY_PI
#	define STORAGE_PREFIX "/usr/share/"
#	define HID_DEV "/dev/hidg0"
#	define DEBUG_STREAMS "/tmp/"
//	#define DEBUG_MSGS
#	define LEDS
#elif ARCHITECTURE == ARCH_ANDROID
#	include <string>
#	define STORAGE_PREFIX "/sdcard/U2F/"
extern std::string hidDev;
#	define HID_DEV hidDev.c_str()
#	define DEBUG_STREAMS "/sdcard/log"
#	define HID_SOCKET
#	define MANUAL_LIFETIME
// #define DEBUG_MSGS
#endif

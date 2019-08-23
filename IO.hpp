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
#include <cstddef>
#include <cstdint>
#include <vector>

extern std::string binaryDirectory;
extern std::string cacheDirectory;

// Returns either the number of bytes specified,
// or returns empty vector without discarding bytes from HID stream
std::vector<uint8_t> readNonBlock(const size_t count);

// Blocking write to HID stream - shouldn't block for too long
void write(const std::vector<uint8_t>& data);

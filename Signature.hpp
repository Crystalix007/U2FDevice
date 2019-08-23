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
#include <array>
#include <vector>

using Digest = std::array<uint8_t, 32>;
using Signature = std::array<uint8_t, 64>;

// Ripped from https://github.com/pratikd650/Teensy_U2F/blob/master/Teensy_U2F.cpp
void appendSignatureAsDER(std::vector<uint8_t>& response, const Signature& signature);

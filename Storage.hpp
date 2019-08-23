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
#include <fstream>
#include <map>
#include <string>

class Storage {
public:
	using KeyHandle = uint32_t;
	using KeyCount = uint32_t;
	using AppParam = std::array<uint8_t, 32>;
	using PrivKey = std::array<uint8_t, 32>;
	using PubKey = std::array<uint8_t, 65>;

protected:
	Storage() = default;

	static std::string filename;

public:
	static void init(const std::string& dirPrefix = "");
	static void init(std::istream& inputStream);
	static void save();
	static void save(std::ostream& outputStream);
	static std::map<KeyHandle, AppParam> appParams;
	static std::map<KeyHandle, PrivKey> privKeys;
	static std::map<KeyHandle, PubKey> pubKeys;
	static std::map<KeyHandle, KeyCount> keyCounts;
};

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
#include <cstdint>
#include <vector>
#include <memory>

struct U2FMessage
{
	public:
		uint32_t        cid;
		uint8_t         cmd;
		std::vector<uint8_t> data;

	public:
		U2FMessage() = default;
		U2FMessage(const uint32_t nCID, const uint8_t nCMD);
		static std::shared_ptr<U2FMessage> readNonBlock();
		void write();
		static void error(const uint32_t tCID, const uint8_t tErr);
};

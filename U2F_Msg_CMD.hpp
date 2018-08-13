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
#include "U2F_CMD.hpp"
#include <cstdint>
#include <map>
#include <vector>
#include <memory>

struct U2F_Msg_CMD : U2F_CMD
{
	uint8_t  cla;
	uint8_t  ins;
	uint8_t  p1;
	uint8_t  p2;
	uint32_t lc;
	uint32_t le;

	const static std::map<uint8_t, bool> usesData;

	protected:
		static uint32_t getLe(const uint32_t byteCount, std::vector<uint8_t> bytes);
		U2F_Msg_CMD() = default;

	public:
		static std::shared_ptr<U2F_Msg_CMD> generate(const std::shared_ptr<U2FMessage> uMsg);
		static void error(const uint32_t channelID, const uint16_t errCode);
		void respond(const uint32_t channelID) const;
};


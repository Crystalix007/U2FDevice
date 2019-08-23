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
#include "U2FMessage.hpp"
#include "U2F_CMD.hpp"
#include <cstdint>
#include <memory>

struct U2F_Ping_CMD : U2F_CMD {
	std::vector<uint8_t> nonce;

public:
	U2F_Ping_CMD(const U2FMessage& uMsg);
	virtual void respond(const uint32_t channelID) const override;
};

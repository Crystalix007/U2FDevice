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
#include "Storage.hpp"
#include "U2F_Msg_CMD.hpp"

struct U2F_Authenticate_APDU : U2F_Msg_CMD {
	uint8_t controlByte;
	std::array<uint8_t, 32> challengeP;
	Storage::AppParam appParam;
	std::vector<uint8_t> keyH;

public:
	U2F_Authenticate_APDU(const U2F_Msg_CMD& msg, const std::vector<uint8_t>& data);

	bool requiresAuthorisation() const override;
	virtual void respond(const uint32_t channelID, bool hasAuthorisation) const override;

	enum ControlCode {
		CheckOnly = 0x07,
		EnforcePresenceSign = 0x03,
		DontEnforcePresenceSign = 0x08
	};
};

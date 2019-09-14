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

class U2F_Register_APDU : public U2F_Msg_CMD {
protected:
	std::array<uint8_t, 32> challengeP;
	Storage::AppParam appP;
	Storage::KeyHandle keyH;

public:
	U2F_Register_APDU(const U2F_Msg_CMD& msg, const std::vector<uint8_t>& data);
	virtual ~U2F_Register_APDU() = default;

	bool requiresAuthorisation() const override;
	void respond(const uint32_t channelID, bool hasAuthorisation) const override;
};

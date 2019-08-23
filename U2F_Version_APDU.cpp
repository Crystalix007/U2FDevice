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

#include "U2F_Version_APDU.hpp"
#include "APDU.hpp"
#include "Field.hpp"
#include "U2FMessage.hpp"
#include "u2f.hpp"
#include <iostream>

using namespace std;

U2F_Version_APDU::U2F_Version_APDU(const U2F_Msg_CMD& msg, const std::vector<uint8_t>& data) {
	// Don't actually respond yet unless invalid
	if (msg.p1 != 0 || msg.p2 != 0)
		throw APDU_STATUS::SW_INS_NOT_SUPPORTED;
	else if (data.size() != 0)
		throw APDU_STATUS::SW_WRONG_LENGTH;
}

void U2F_Version_APDU::respond(const uint32_t channelID) const {
	char ver[]{ 'U', '2', 'F', '_', 'V', '2' };
	U2FMessage m{};

	m.cid = channelID;
	m.cmd = U2FHID_MSG;
	m.data.insert(m.data.end(), FIELD(ver));
	auto sC = APDU_STATUS::SW_NO_ERROR;
	m.data.insert(m.data.end(), FIELD_BE(sC));
	m.write();
}

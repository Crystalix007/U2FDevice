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

#include "U2F_Init_CMD.hpp"
#include "Field.hpp"
#include "u2f.hpp"
#include <stdexcept>

using namespace std;

U2F_Init_CMD::U2F_Init_CMD(const U2FMessage& uMsg) {
	if (uMsg.cmd != U2FHID_INIT)
		throw runtime_error{ "Failed to get U2F Init message" };
	else if (uMsg.cid != CID_BROADCAST) {
		U2FMessage::error(uMsg.cid, ERR_OTHER);
		throw runtime_error{ "Invalid CID for init command" };
	} else if (uMsg.data.size() != INIT_NONCE_SIZE) {
		U2FMessage::error(uMsg.cid, ERR_INVALID_LEN);
		throw runtime_error{ "Init nonce is incorrect size" };
	}

	this->nonce = *reinterpret_cast<const uint64_t*>(uMsg.data.data());
}

void U2F_Init_CMD::respond(const uint32_t channelID, bool) const {
	U2FMessage msg{};
	msg.cid = CID_BROADCAST;
	msg.cmd = U2FHID_INIT;

	msg.data.insert(msg.data.end(), FIELD(this->nonce));
	msg.data.insert(msg.data.end(), FIELD(channelID));
	msg.data.push_back(2);            // Protocol version
	msg.data.push_back(1);            // Major device version
	msg.data.push_back(0);            // Minor device version
	msg.data.push_back(1);            // Build device version
	msg.data.push_back(CAPFLAG_WINK); // Wink capability

	msg.write();
}

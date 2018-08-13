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

#include "U2F_Ping_CMD.hpp"
#include "u2f.hpp"

using namespace std;

U2F_Ping_CMD::U2F_Ping_CMD(const shared_ptr<U2FMessage> uMsg)
	: nonce{ uMsg->data }
{
	if (uMsg->cmd != U2FHID_PING)
		throw runtime_error{ "Failed to get U2F ping message" };
}

void U2F_Ping_CMD::respond(const uint32_t channelID) const
{
	U2FMessage msg{};
	msg.cid = channelID;
	msg.cmd = U2FHID_PING;
	msg.data = nonce;
	msg.write();
}

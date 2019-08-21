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

#include "U2F_CMD.hpp"
#include "u2f.hpp"
#include "U2F_Msg_CMD.hpp"
#include "U2F_Init_CMD.hpp"
#include "U2F_Ping_CMD.hpp"

using namespace std;

shared_ptr<U2F_CMD> U2F_CMD::get(const U2FMessage& uMsg)
{
	try
	{
		switch (uMsg.cmd)
		{
			case U2FHID_PING:
				return make_shared<U2F_Ping_CMD>(uMsg);
			case U2FHID_MSG:
				return U2F_Msg_CMD::generate(uMsg);
			case U2FHID_INIT:
				return make_shared<U2F_Init_CMD>(uMsg);
			default:
				U2FMessage::error(uMsg.cid, ERR_INVALID_CMD);
				return {};
		}
	}
	catch (runtime_error& ignored)
	{
		U2FMessage::error(uMsg.cid, ERR_OTHER);
		return {};
	}
}

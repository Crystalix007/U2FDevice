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

#include "Channel.hpp"
#include "U2F_CMD.hpp"
#include "u2f.hpp"
#include <iostream>
#include <stdexcept>
#include <android/log.h>

using namespace std;

Channel::Channel(const uint32_t channelID)
    : cid{ channelID }, initState{ ChannelInitState::Unitialised }, lockedState{
	      ChannelLockedState::Unlocked
      } {}

uint32_t Channel::getCID() const {
	return cid;
}

bool Channel::handle(const U2FMessage& uMsg, AuthorisationLevel auth) {
	if (uMsg.cmd == U2FHID_INIT)
		this->initState = ChannelInitState::Initialised;
	else if (uMsg.cid != this->cid)
		throw runtime_error{ "CID of request invalid for this channel" };

	if (this->initState != ChannelInitState::Initialised)
		throw runtime_error{ "Channel in incorrect (uninitialized) state to handle request" };
	else if (this->lockedState != ChannelLockedState::Unlocked)
		throw runtime_error{ "Channel in incorrect (locked) state to handle request" };

	auto cmd = U2F_CMD::get(uMsg);

	if (!cmd)
		return true;

	if (cmd->requiresAuthorisation()) {
		if (auth == AuthorisationLevel::Unspecified) {
			__android_log_print(ANDROID_LOG_DEBUG, "U2FDevice",
								"Action requires authorisation; seeking authorisation");
			return false;
		}
		else if (auth == AuthorisationLevel::Unauthorised)
			__android_log_print(ANDROID_LOG_DEBUG, "U2FDevice", "Action requires authorisation; authorisation not granted");
	}

	cmd->respond(this->cid, auth == AuthorisationLevel::Authorised);
	return true;
}

void Channel::init(const ChannelInitState newInitState) {
	this->initState = newInitState;
}

void Channel::lock(const ChannelLockedState newLockedState) {
	this->lockedState = newLockedState;
}

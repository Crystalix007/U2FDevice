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
#include <cstdint>
#include <memory>

enum class ChannelInitState { Unitialised, Initialised };

enum class ChannelLockedState { Locked, Unlocked };

enum class AuthorisationLevel { Unspecified, Unauthorised, Authorised };

class Channel {
protected:
	uint32_t cid;
	ChannelInitState initState;
	ChannelLockedState lockedState;

public:
	Channel(const uint32_t channelID);

	// Returns false if requires authorisation check
	// True otherwise
	bool handle(const U2FMessage& uMsg, AuthorisationLevel auth);

	uint32_t getCID() const;
	void init(const ChannelInitState newInitState);
	void lock(const ChannelLockedState newLockedState);
};

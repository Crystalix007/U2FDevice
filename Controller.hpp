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
#include "Channel.hpp"
#include <chrono>
#include <map>

class Controller {
protected:
	std::map<uint32_t, Channel> channels;
	uint32_t currChannel;
	std::chrono::system_clock::time_point lastMessage;

public:
	Controller(const uint32_t startChannel = 1);

	void handleTransaction();

	// Returns false if required authentication
	// Returns true otherwise
	bool handleTransaction(const U2FMessage& msg, AuthorisationLevel auth);
	uint32_t nextChannel();
};

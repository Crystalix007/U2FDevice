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

#include "Controller.hpp"
#include "IO.hpp"
#include "LED.hpp"
#include "u2f.hpp"
#include <iostream>

using namespace std;

Controller::Controller(const uint32_t startChannel) : channels{}, currChannel{ startChannel } {}

void Controller::handleTransaction() {
	auto msg = U2FMessage::readNonBlock();

	if (!msg)
		return;

	handleTransaction(*msg);
}

void Controller::handleTransaction(const U2FMessage& msg) {
	try {
		if (channels.size() != 0 &&
		    chrono::duration_cast<chrono::seconds>(chrono::system_clock::now() - lastMessage) <
		        chrono::seconds(5))
			toggleACTLED();
		else
			enableACTLED(false);
	} catch (runtime_error& ignored) {
	}

	lastMessage = chrono::system_clock::now();

	auto opChannelID = msg.cid;

	if (msg.cmd == U2FHID_INIT) {
		opChannelID = nextChannel();
		auto channel = Channel{ opChannelID };
		try {
			channels.emplace(opChannelID, channel); // In case of wrap-around replace existing one
		} catch (...) {
			channels.insert(make_pair(opChannelID, channel));
		}
	} else if (channels.find(opChannelID) == channels.end()) {
		U2FMessage::error(opChannelID, ERR_CHANNEL_BUSY);
		return;
	}

#ifdef DEBUG_MSGS
	clog << "Message:" << endl;
	clog << "cid: " << msg.cid << ", cmd: " << static_cast<unsigned int>(msg.cmd) << endl;
#endif

	channels.at(opChannelID).handle(msg);
}

uint32_t Controller::nextChannel() {
	do
		currChannel++;
	while (currChannel == 0xFFFFFFFF || currChannel == 0);

	return currChannel;
}

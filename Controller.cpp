#include "Controller.hpp"
#include "u2f.hpp"
#include <iostream>
#include "IO.hpp"
#include "LED.hpp"

using namespace std;

Controller::Controller(const uint32_t startChannel)
	: channels{}, currChannel{ startChannel }
{}

void Controller::handleTransaction()
{
	try
	{
		if (channels.size() != 0 && chrono::duration_cast<chrono::seconds>(chrono::system_clock::now() - lastMessage) < 5s)
			toggleACTLED();
		else
			enableACTLED(false);
		}
	catch (runtime_error)
	{}

	auto msg = U2FMessage::readNonBlock();

	if (!msg)
		return;
	
	lastMessage = chrono::system_clock::now();

	auto opChannel = msg->cid;

	if (msg->cmd == U2FHID_INIT)
	{
		opChannel = nextChannel();
		auto channel = Channel{ opChannel };

		try
		{
			channels.emplace(opChannel, channel); //In case of wrap-around replace existing one
		}
		catch (...)
		{
			channels.insert(make_pair(opChannel, channel));
		}
	}

	channels.at(opChannel).handle(msg);
}

uint32_t Controller::nextChannel()
{
	do
		currChannel++;
	while (currChannel == 0xFFFFFFFF || currChannel == 0);

	return currChannel;
}

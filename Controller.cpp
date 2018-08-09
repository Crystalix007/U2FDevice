#include "Controller.hpp"
#include "u2f.hpp"
#include <iostream>
#include "IO.hpp"

using namespace std;

Controller::Controller(const uint32_t startChannel)
	: channels{}, currChannel{ startChannel }
{}

void Controller::handleTransaction()
{
	auto msg = U2FMessage::readNonBlock();

	if (!msg)
		return;

	auto opChannel = msg->cid;

	clog << "Got msg with cmd of: " << static_cast<uint16_t>(msg->cmd) << endl;

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

const uint32_t Controller::nextChannel()
{
	do
		currChannel++;
	while (currChannel == 0xFFFFFFFF || currChannel == 0);

	return currChannel;
}

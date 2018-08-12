#include "Channel.hpp"
#include <stdexcept>
#include "u2f.hpp"
#include "U2F_CMD.hpp"
#include <iostream>

using namespace std;

Channel::Channel(const uint32_t channelID)
	: cid{ channelID }, initState{ ChannelInitState::Unitialised }, lockedState{ ChannelLockedState::Unlocked }
{}

uint32_t Channel::getCID() const
{
	return cid;
}

void Channel::handle(const shared_ptr<U2FMessage> uMsg)
{
	if (uMsg->cmd == U2FHID_INIT)
		this->initState = ChannelInitState::Initialised;
	else if (uMsg->cid != this->cid)
		throw runtime_error{ "CID of request invalid for this channel" };

	if (this->initState != ChannelInitState::Initialised)
		throw runtime_error{ "Channel in incorrect (uninitialized) state to handle request" };
	else if (this->lockedState != ChannelLockedState::Unlocked)
		throw runtime_error{ "Channel in incorrect (locked) state to handle request" };

	auto cmd = U2F_CMD::get(uMsg);

	if (cmd)
		return cmd->respond(this->cid);
}

void Channel::init(const ChannelInitState newInitState)
{
	this->initState = newInitState;
}

void Channel::lock(const ChannelLockedState newLockedState)
{
	this->lockedState = newLockedState;
}

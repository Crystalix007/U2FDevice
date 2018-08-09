#pragma once
#include <cstdint>
#include <memory>
#include "U2FMessage.hpp"

enum class ChannelInitState
{
	Unitialised,
	Initialised
};

enum class ChannelLockedState
{
	Locked,
	Unlocked
};

class Channel
{
	protected:
		uint32_t cid;
		ChannelInitState   initState;
		ChannelLockedState lockedState;

	public:
		Channel(const uint32_t channelID);
		void handle(const std::shared_ptr<U2FMessage> uMsg);
		
		uint32_t getCID() const;
		void init(const ChannelInitState newInitState);
		void lock(const ChannelLockedState newLockedState);
};

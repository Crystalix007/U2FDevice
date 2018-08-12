#pragma once
#include <map>
#include <chrono>
#include "Channel.hpp"

class Controller
{
	protected:
		std::map<uint32_t, Channel>               channels;
		uint32_t                                  currChannel;
		std::chrono::system_clock::time_point     lastMessage; 

	public:
		Controller(const uint32_t startChannel = 1);

		void handleTransaction();
		uint32_t nextChannel();
};

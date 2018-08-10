#pragma once
#include <map>
#include "Channel.hpp"

class Controller
{
	protected:
		std::map<uint32_t, Channel> channels;
		uint32_t currChannel;

	public:
		Controller(const uint32_t startChannel = 1);

		void handleTransaction();
		uint32_t nextChannel();
};

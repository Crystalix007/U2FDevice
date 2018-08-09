#pragma once
#include <memory>
#include "U2FMessage.hpp"

struct U2F_CMD
{
	protected:
		U2F_CMD() = default;

	public:
		virtual ~U2F_CMD() = default;
		static std::shared_ptr<U2F_CMD> get(const std::shared_ptr<U2FMessage> uMsg);
		virtual void respond(const uint32_t channelID) const = 0;
}; //For polymorphic type casting

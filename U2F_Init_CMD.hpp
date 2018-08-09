#pragma once
#include <cstdint>
#include <memory>
#include "U2F_CMD.hpp"
#include "U2FMessage.hpp"

struct U2F_Init_CMD : U2F_CMD
{
	uint64_t nonce;

	public:
		U2F_Init_CMD(const std::shared_ptr<U2FMessage> uMsg);
		virtual void respond(const uint32_t channelID) const override;
};

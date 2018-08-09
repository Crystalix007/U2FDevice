#pragma once
#include "U2F_Msg_CMD.hpp"
#include "Storage.hpp"

struct U2F_Register_APDU : U2F_Msg_CMD
{
	std::array<uint8_t, 32> challengeP;
	Storage::AppParam       appP;
	Storage::KeyHandle      keyH;

	public:
		U2F_Register_APDU(const U2F_Msg_CMD &msg, const std::vector<uint8_t> &data);

		void respond(const uint32_t channelID) const override;
};


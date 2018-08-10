#pragma once
#include "U2F_Msg_CMD.hpp"

struct U2F_Version_APDU : U2F_Msg_CMD
{
	public:
		U2F_Version_APDU(const U2F_Msg_CMD &msg, const std::vector<uint8_t> &data);
		void respond(const uint32_t channelID) const override;
};

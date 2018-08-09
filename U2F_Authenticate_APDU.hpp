#pragma once
#include "U2F_Msg_CMD.hpp"
#include "Storage.hpp"

struct U2F_Authenticate_APDU : U2F_Msg_CMD
{
	uint8_t controlByte;
	std::array<uint8_t, 32> challengeP;
	Storage::AppParam       appParam;
	std::vector<uint8_t>    keyH;

	public:
		U2F_Authenticate_APDU(const U2F_Msg_CMD &msg, const std::vector<uint8_t> &data);

		virtual void respond(const uint32_t channelID) const override;

	enum ControlCode
	{
		CheckOnly = 0x07,
		EnforcePresenceSign = 0x03,
		DontEnforcePresenceSign = 0x08
	};
};

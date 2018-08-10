#pragma once
#include "U2F_CMD.hpp"
#include <cstdint>
#include <map>
#include <vector>
#include <memory>

struct U2F_Msg_CMD : U2F_CMD
{
	uint8_t  cla;
	uint8_t  ins;
	uint8_t  p1;
	uint8_t  p2;
	uint32_t lc;
	uint32_t le;

	const static std::map<uint8_t, bool> usesData;

	protected:
		static uint32_t getLe(const uint32_t byteCount, std::vector<uint8_t> bytes);
		U2F_Msg_CMD() = default;

	public:
		static std::shared_ptr<U2F_Msg_CMD> generate(const std::shared_ptr<U2FMessage> uMsg);
		static void error(const uint32_t channelID, const uint16_t errCode);
		void respond(const uint32_t channelID) const;
};


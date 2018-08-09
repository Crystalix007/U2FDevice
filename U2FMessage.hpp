#pragma once
#include <cstdint>
#include <vector>
#include <memory>

struct U2FMessage
{
	public:
		uint32_t        cid;
		uint8_t         cmd;
		std::vector<uint8_t> data;

	public:
		U2FMessage() = default;
		U2FMessage(const uint32_t nCID, const uint8_t nCMD);
		static std::shared_ptr<U2FMessage> readNonBlock();
		void write();
	
	protected:
		static void error(const uint32_t tCID, const uint16_t tErr);
};

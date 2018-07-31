#pragma once
#include <cstdint>
#include <memory>
#include <array>
#include "Constants.hpp"

struct Packet
{
	uint32_t            cid;
	uint8_t buf[packetSize];

	protected:
		Packet() = default;
		virtual void writePacket();

	public:
		static std::shared_ptr<Packet> getPacket();
		virtual ~Packet() = default;
};

struct InitPacket : Packet
{
	uint8_t                             cmd;
	uint8_t                             bcnth;
	uint8_t                             bcntl;
	std::array<uint8_t, packetSize - 7> data{};

	public:
		InitPacket() = default;
		static std::shared_ptr<InitPacket> getPacket(const uint32_t rCID, const uint8_t rCMD);
		void writePacket() override;
};

struct ContPacket : Packet
{
	uint8_t                             seq;
	std::array<uint8_t, packetSize - 5> data{};

	public:
		ContPacket() = default;
		static std::shared_ptr<ContPacket> getPacket(const uint32_t rCID, const uint8_t rSeq);
		void writePacket() override;
};

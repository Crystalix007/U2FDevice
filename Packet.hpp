/*
U2FDevice - A program to allow Raspberry Pi Zeros to act as U2F tokens
Copyright (C) 2018  Michael Kuc

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once
#include <cstdint>
#include <memory>
#include <array>
#include "u2f.hpp"

struct Packet
{
	public:
		uint32_t cid;
		uint8_t  buf[HID_RPT_SIZE];

	protected:
		Packet() = default;
		virtual void writePacket();

	public:
		static std::shared_ptr<Packet> getPacket();
		virtual ~Packet() = default;
};

struct InitPacket : Packet
{
	public:
		uint8_t                               cmd;
		uint8_t                               bcnth;
		uint8_t                               bcntl;
		std::array<uint8_t, HID_RPT_SIZE - 7> data{};

	public:
		InitPacket() = default;
		static std::shared_ptr<InitPacket> getPacket(const uint32_t rCID, const uint8_t rCMD);
		void writePacket() override;
};

struct ContPacket : Packet
{
	public:
		uint8_t                             seq;
		std::array<uint8_t, HID_RPT_SIZE - 5> data{};

	public:
		ContPacket() = default;
		static std::shared_ptr<ContPacket> getPacket(const uint32_t rCID, const uint8_t rSeq);
		void writePacket() override;
};

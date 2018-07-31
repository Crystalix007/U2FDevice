#pragma once
#include <cstdint>
#include <vector>

struct U2FMessage
{
	uint32_t        cid;
	uint8_t         cmd;
	std::vector<uint8_t> data;

	static U2FMessage read();
	
	void write();
};

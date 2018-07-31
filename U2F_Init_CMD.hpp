#pragma once
#include <cstdint>
#include "U2F_CMD.hpp"

struct U2F_Init_CMD : U2F_CMD
{
	uint64_t nonce;

	public:
		static U2F_Init_CMD get();
};

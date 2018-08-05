#pragma once
#include "Field.hpp"

template <typename Type>
std::vector<uint8_t> beEncode(const Type val)
{
	return beEncode(reinterpret_cast<const uint8_t *>(&val), sizeof(val));
}

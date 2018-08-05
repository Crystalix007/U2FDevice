#pragma once
#include <vector>
#include <cstdint>

template <typename Type>
std::vector<uint8_t> beEncode(const Type val);

std::vector<uint8_t> beEncode(const uint8_t* val, const std::size_t byteCount);

#define FIELD(name) reinterpret_cast<const uint8_t*>(&name), (reinterpret_cast<const uint8_t*>(&name) + sizeof(name))
#define FIELD_BE(name) reverse_iterator<const uint8_t*>(reinterpret_cast<const uint8_t*>(&name) + sizeof(name)), reverse_iterator<const uint8_t*>(reinterpret_cast<const uint8_t*>(&name))

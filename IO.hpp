#pragma once
#include <cstddef>
#include <cstdint>
#include <vector>

//Returns either the number of bytes specified,
//or returns empty vector without discarding bytes from HID stream
std::vector<uint8_t> readNonBlock(const size_t count);

//Blocking write to HID stream - shouldn't block for too long
void write(const uint8_t* bytes, const size_t count);

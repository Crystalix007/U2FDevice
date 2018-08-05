#pragma once
#include <vector>
#include <array>

using Digest    = std::array<uint8_t, 32>;
using Signature = std::array<uint8_t, 64>;

//Ripped from https://github.com/pratikd650/Teensy_U2F/blob/master/Teensy_U2F.cpp
void appendSignatureAsDER(std::vector<uint8_t> &response, const Signature &signature);

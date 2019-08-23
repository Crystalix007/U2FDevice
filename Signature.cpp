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

#include "Signature.hpp"
#include <iostream>

using namespace std;

// Ripped from https://github.com/pratikd650/Teensy_U2F/blob/master/Teensy_U2F.cpp
void appendSignatureAsDER(vector<uint8_t>& response, const array<uint8_t, 64>& signature) {
	response.push_back(0x30); // Start of ASN.1 SEQUENCE
	response.push_back(68);   // total length from (2 * (32 + 2)) to (2 * (33 + 2))
	size_t countByte = response.size() - 1;

	// Loop twice - for R and S
	for (unsigned int i = 0; i < 2; i++) {
		unsigned int sigOffs = i * 32;
		auto offset = signature.begin() + sigOffs;
		response.push_back(0x02); // header: integer
		response.push_back(32);   // 32 byte
		if (signature[sigOffs] > 0x7f) {
			// Integer needs to be represented in 2's completement notion
			response.back()++;
			response.push_back(0); // add leading 0, to indicate it is a positive number
			response[countByte]++;
		}
		copy(offset, offset + 32, back_inserter(response)); // R or S value
	}
}

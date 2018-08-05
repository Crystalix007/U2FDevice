#include "Signature.hpp"
#include <iostream>

using namespace std;

//Ripped from https://github.com/pratikd650/Teensy_U2F/blob/master/Teensy_U2F.cpp
void appendSignatureAsDER(vector<uint8_t> &response, const array<uint8_t, 64> &signature)
{
	response.push_back(0x30); // Start of ASN.1 SEQUENCE
	response.push_back(68);   //total length from (2 * (32 + 2)) to (2 * (33 + 2))
	size_t countByte = response.size() - 1;

	// Loop twice - for R and S
	for(unsigned int i = 0; i < 2; i++)
	{
		unsigned int sigOffs = i * 32;
		auto offset = signature.begin() + sigOffs;
		response.push_back(0x02);  //header: integer
		response.push_back(32);    //32 byte
		if (signature[sigOffs] > 0x7f)
		{
			// Integer needs to be represented in 2's completement notion
			response.back()++;
			response.push_back(0); // add leading 0, to indicate it is a positive number
			response[countByte]++;
		}
		copy(offset, offset + 32, back_inserter(response)); //R or S value
	}
}

#include "U2F_Register_APDU.hpp"
#include <exception>
#include <cstring>
#include "micro-ecc/uECC.h"
#include "Certificates.hpp"
#include <mbedtls/sha256.h>
#include <iostream>

using namespace std;

U2F_Register_APDU::U2F_Register_APDU(const U2F_Msg_CMD &msg, const vector<uint8_t> &data)
	: U2F_Msg_CMD{ msg }
{
	if (data.size() != 64)
		throw runtime_error{ "Incorrect registration size" };
	else if (p1 != 0x00 || p2 != 0x00)
	{
		cerr << "p1: " << static_cast<uint32_t>(p1) << ", p2: " << static_cast<uint32_t>(p2) << endl;
		//throw runtime_error{ "Invalid APDU parameters" };
	}

	copy(data.data() + 0,  data.data() + 32, challengeP.begin());
	copy(data.data() + 32, data.data() + 64, appP.begin());

	//Use crypto lib to generate keypair
	
	uint8_t pub[65];
	uint8_t priv[32];

	//Unsure if necessary
	//From github.com/pratikd650/Teensy_U2F/blob/master/Teensy_U2F.cpp
	pub[0] = 0x04;

	uECC_make_key(pub + 1, priv, uECC_secp256r1());

	Storage::PrivKey privKey{};
	copy(priv, end(priv), privKey.begin());
	Storage::PubKey pubKey;
	copy(pub,  end(pub), pubKey.begin()); 

	this->keyH = Storage::appParams.size();
	Storage::appParams[this->keyH] = appP;
	Storage::privKeys[this->keyH]  = privKey;
	Storage::pubKeys[this->keyH]   = pubKey;
}

//Ripped from https://github.com/pratikd650/Teensy_U2F/blob/master/Teensy_U2F.cpp
void appendSignatureAsDER(vector<uint8_t> &response, const array<uint8_t, 64> &signature) {
	response.push_back(0x30); // Start of ASN.1 SEQUENCE
	response.push_back(68);   //total length from (2 * (32 + 2)) to (2 * (33 + 2))
	auto &count = response.back();

	// Loop twice - for R and S
	for(unsigned int i = 0; i < 2; i++) {
		unsigned int sigOffs = i * 32;
		response.push_back(0x02);  //header: integer
		response.push_back(32);    //32 byte
		if (signature[sigOffs] > 0x7f) { // Integer needs to be represented in 2's completement notion
			response.back()++;
			response.push_back(0); // add leading 0, to indicate it is a positive number
			count++;
		}
		copy(signature.begin() + sigOffs, signature.begin() + sigOffs + 32, back_inserter(response)); //R or S value
	}
}

void U2F_Register_APDU::respond()
{
	const auto appParam = Storage::appParams[this->keyH];
	const auto pubKey   = Storage::pubKeys[this->keyH];
	const auto privKey  = Storage::privKeys[this->keyH];

	vector<uint8_t> response;
	response.push_back(0x05);
	copy(pubKey.begin(), pubKey.end(), back_inserter(response));
	response.push_back(sizeof(this->keyH));
	auto fakeKeyHBytes = reinterpret_cast<uint8_t *>(&this->keyH);
	copy(fakeKeyHBytes, fakeKeyHBytes + 4, back_inserter(response));
	copy(attestCert, end(attestCert), back_inserter(response));

	//Gen signature
	array<uint8_t, 32> digest;
	{
		mbedtls_sha256_context shaContext;

		mbedtls_sha256_init(&shaContext);
		mbedtls_sha256_starts(&shaContext, 0);

		uint8_t byteReserved = 0;
		mbedtls_sha256_update(&shaContext, reinterpret_cast<unsigned char*>(&byteReserved), 1);

		mbedtls_sha256_update(&shaContext, reinterpret_cast<const unsigned char*>(appParam.data()), appParam.size());

		mbedtls_sha256_update(&shaContext, reinterpret_cast<unsigned char*>(challengeP.data()), challengeP.size());

		mbedtls_sha256_update(&shaContext, reinterpret_cast<unsigned char*>(&keyH), sizeof(keyH));

		mbedtls_sha256_update(&shaContext, reinterpret_cast<const unsigned char*>(pubKey.data()), pubKey.size());

		mbedtls_sha256_finish(&shaContext, reinterpret_cast<unsigned char*>(digest.data()));
		mbedtls_sha256_free(&shaContext);
	}

	array<uint8_t, 64> signature;
	uECC_sign(attestPrivKey, digest.data(), digest.size(), signature.data(), uECC_secp256r1());

	//Append signature as DER
	appendSignatureAsDER(response, signature);
}

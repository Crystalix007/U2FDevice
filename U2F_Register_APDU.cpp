#include "U2F_Register_APDU.hpp"
#include <exception>
#include <cstring>
#include "micro-ecc/uECC.h"
#include "Certificates.hpp"
#include <mbedtls/sha256.h>
#include <iostream>
#include "APDU.hpp"
#include "U2FMessage.hpp"
#include "u2f.hpp"
#include "Signature.hpp"

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
	Storage::PrivKey privKey{};
	Storage::PubKey pubKey{};
	
	//Unsure if necessary
	//From github.com/pratikd650/Teensy_U2F/blob/master/Teensy_U2F.cpp
	pubKey[0] = 0x04;

	uECC_make_key(pubKey.data() + 1, privKey.data(), uECC_secp256r1());

	this->keyH = Storage::appParams.size(); //To increment the key handle
	Storage::appParams[this->keyH] = appP;
	Storage::privKeys[this->keyH]  = privKey;
	Storage::pubKeys[this->keyH]   = pubKey;
	Storage::keyCounts[this->keyH] = 0;

	clog << "Produced pub key: " << hex;

	for (auto b : pubKey)
		clog << static_cast<uint32_t>(b) << ' ';

	clog << endl << dec << "Got U2F_Reg request" << endl;
}

void U2F_Register_APDU::respond(const uint32_t channelID) const
{
	U2FMessage m{};
	m.cid = channelID;
	m.cmd = U2FHID_MSG;

	auto& response = m.data;
	const auto appParam = Storage::appParams[this->keyH];
	const auto pubKey   = Storage::pubKeys[this->keyH];
	const auto privKey  = Storage::privKeys[this->keyH];

	response.push_back(0x05);
	copy(pubKey.begin(), pubKey.end(), back_inserter(response));
	response.push_back(sizeof(this->keyH));

	auto fakeKeyHBytes = reinterpret_cast<const uint8_t *>(&this->keyH);
	copy(fakeKeyHBytes, fakeKeyHBytes + sizeof(this->keyH), back_inserter(response));

	copy(attestCert, end(attestCert), back_inserter(response));

	//Gen signature
	Digest digest;
	{
		mbedtls_sha256_context shaContext;

		mbedtls_sha256_init(&shaContext);
		mbedtls_sha256_starts(&shaContext, 0);

		uint8_t byteReserved = 0;
		mbedtls_sha256_update(&shaContext, reinterpret_cast<unsigned char*>(&byteReserved), 1);

		mbedtls_sha256_update(&shaContext, reinterpret_cast<const unsigned char*>(appParam.data()), appParam.size());

		mbedtls_sha256_update(&shaContext, reinterpret_cast<const unsigned char*>(challengeP.data()), challengeP.size());

		mbedtls_sha256_update(&shaContext, reinterpret_cast<const unsigned char*>(&keyH), sizeof(keyH));

		mbedtls_sha256_update(&shaContext, reinterpret_cast<const unsigned char*>(pubKey.data()), pubKey.size());

		mbedtls_sha256_finish(&shaContext, digest.data());
		mbedtls_sha256_free(&shaContext);
	}

	Signature signature;
	std::clog << "Will sign digest with priv key" << std::endl;
	uECC_sign(attestPrivKey, digest.data(), digest.size(), signature.data(), uECC_secp256r1());

	//Append signature as DER
	std::clog << "Will append sig as DER" << std::endl;
	appendSignatureAsDER(response, signature);

	response.push_back(static_cast<uint16_t>(APDU_STATUS::SW_NO_ERROR) >> 8);
	response.push_back(static_cast<uint16_t>(APDU_STATUS::SW_NO_ERROR) & 0xff);

	std::clog << "Writing out " << response.size() << " bytes in response" << std::endl;

	m.write();
}

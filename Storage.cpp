#include "Storage.hpp"
#include <exception>
#include <sstream>
#include "Base64.tpp"
#include <iostream>

using namespace std;

std::string Storage::filename{};
std::map<Storage::KeyHandle, Storage::AppParam> Storage::appParams{};
std::map<Storage::KeyHandle, Storage::PrivKey>  Storage::privKeys{};
std::map<Storage::KeyHandle, Storage::PubKey>   Storage::pubKeys{};
std::map<Storage::KeyHandle, Storage::KeyCount> Storage::keyCounts{};

void Storage::init(const string &dirPrefix)
{
	Storage::filename = dirPrefix + "U2F_Priv_Keys.txt";
	ifstream file{ Storage::filename };
	string line;
	size_t lineNumber = 0;

	while (getline(file, line))
	{
		auto strLineNum = to_string(lineNumber);
		stringstream ss{ line };
		string keyHStr,   appStr,   privStr,   pubStr,   keyCStr;
		ss >>  keyHStr >> appStr >> privStr >> pubStr >> keyCStr;

		if (!ss)
			throw runtime_error{ string{ "Invalid syntax of line " } + strLineNum };

		char *endP = nullptr;
		auto keyH{ static_cast<Storage::KeyHandle>(strtoull(keyHStr.c_str(), &endP, 10)) };

		if (!endP)
			throw runtime_error{ "Invalid keyhandle format on line " + strLineNum };
		
		endP = nullptr;
		auto keyC{ static_cast<Storage::KeyCount>(strtoull(keyCStr.c_str(), &endP, 10)) };

		if (!endP)
			throw runtime_error{ "Invalid key count format on line " + strLineNum };

		Storage::AppParam appParam{};
		b64decode(appStr, appParam);

		Storage::PrivKey  privKey{};
		b64decode(privStr, privKey);

		Storage::PubKey   pubKey{};
		b64decode(pubStr, pubKey);

		clog << "Loaded key with pubkey: " << hex;

		for (auto b : pubKey)
			clog << static_cast<uint32_t>(b) << ' ';

		clog << dec << endl;

		Storage::appParams[keyH] = appParam;
		Storage::privKeys[keyH]  = privKey;
		Storage::pubKeys[keyH]   = pubKey;
		Storage::keyCounts[keyH] = keyC;

		lineNumber++;
	}
}

void Storage::save()
{
	ofstream file{ Storage::filename };

	for (auto &keypair : Storage::appParams)
	{
		const auto& keyID    = keypair.first;
		const auto& appParam = keypair.second;
		const auto& privKey  = Storage::privKeys[keyID];
		const auto& pubKey   = Storage::pubKeys[keyID];
		const auto& keyCount = Storage::keyCounts[keyID];

		file << keyID;
		file << ' ';

		string appPStr{};
		b64encode(appParam, appPStr);
		file << appPStr << ' ';

		string privKStr{};
		b64encode(privKey, privKStr);
		file << privKStr << ' ';

		string pubKStr{};
		b64encode(pubKey, pubKStr);
		file << pubKStr << ' ';

		file << keyCount << endl;
	}
}

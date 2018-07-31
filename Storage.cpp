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

void Storage::init(const string &dirPrefix)
{
	Storage::filename = dirPrefix + "U2F_Priv_Keys.txt";
	ifstream file{ Storage::filename };
	string line;
	size_t lineNumber = 0;

	base64::decoder decoder{};
	
	while (getline(file, line))
	{
		auto strLineNum = to_string(lineNumber);
		stringstream ss{ line };
		string keyHStr,   appStr,   privStr,   pubStr;
		ss >>  keyHStr >> appStr >> privStr >> pubStr;

		if (!ss)
			throw runtime_error{ string{ "Invalid syntax of line " } + strLineNum };

		char *endP = nullptr;
		Storage::KeyHandle keyH{ strtoull(keyHStr.c_str(), &endP, 10) };

		if (!endP)
			throw runtime_error{ "Invalid keyhandle format on line " + strLineNum };

		//if (appStr.size() != 32)
		//	throw runtime_error{ "Invalid length of app parameter on line " + strLineNum };
		//if (privStr.size() != 32)
		//	throw runtime_error{ "Invalid length of private key on line " + strLineNum };
		//if (pubStr.size()  != 65)
		//	throw runtime_error{ "Invalid length of public key on line " + strLineNum };

		Storage::AppParam appParam{};
		b64decode(appStr, appParam);

		Storage::PrivKey  privKey{};
		b64decode(privStr, privKey);

		Storage::PubKey   pubKey{};
		b64decode(pubStr, pubKey);

		Storage::appParams[keyH] = appParam;
		Storage::privKeys[keyH]  = privKey;
		Storage::pubKeys[keyH]   = pubKey;

		lineNumber++;
	}
}

void Storage::save()
{
	ofstream file{ Storage::filename };
	base64::encoder encoder{};

	for (auto &keypair : Storage::appParams)
	{
		const auto keyID = keypair.first;
		const auto appParam = keypair.second;
		const auto privKey = Storage::privKeys[keypair.first];
		const auto pubKey = Storage::pubKeys[keypair.first];

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
		file << pubKStr << endl;
	}
}

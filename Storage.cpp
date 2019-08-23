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

#include "Storage.hpp"
#include "Base64.tpp"
#include <exception>
#include <iostream>
#include <sstream>

using namespace std;

std::string Storage::filename{};
std::map<Storage::KeyHandle, Storage::AppParam> Storage::appParams{};
std::map<Storage::KeyHandle, Storage::PrivKey> Storage::privKeys{};
std::map<Storage::KeyHandle, Storage::PubKey> Storage::pubKeys{};
std::map<Storage::KeyHandle, Storage::KeyCount> Storage::keyCounts{};

void Storage::init(const string& dirPrefix) {
	Storage::filename = dirPrefix + "U2F_Priv_Keys.txt";
	ifstream file{ Storage::filename };

	init(file);
}

void Storage::init(std::istream& inputStream) {
	string line;
	size_t lineNumber = 0;

	while (getline(inputStream, line)) {
		auto strLineNum = to_string(lineNumber);
		stringstream ss{ line };
		string keyHStr, appStr, privStr, pubStr, keyCStr;
		ss >> keyHStr >> appStr >> privStr >> pubStr >> keyCStr;

		if (!ss)
			throw runtime_error{ string{ "Invalid syntax of line " } + strLineNum };

		char* endP = nullptr;
		Storage::KeyHandle keyH{ static_cast<Storage::KeyHandle>(
			strtoull(keyHStr.c_str(), &endP, 10)) };

		if (!endP)
			throw runtime_error{ "Invalid keyhandle format on line " + strLineNum };

		endP = nullptr;
		Storage::KeyCount keyC{ static_cast<Storage::KeyCount>(
			strtoull(keyCStr.c_str(), &endP, 10)) };

		if (!endP)
			throw runtime_error{ "Invalid key count format on line " + strLineNum };

		Storage::AppParam appParam{};
		b64decode(appStr, appParam);

		Storage::PrivKey privKey{};
		b64decode(privStr, privKey);

		Storage::PubKey pubKey{};
		b64decode(pubStr, pubKey);

		Storage::appParams[keyH] = appParam;
		Storage::privKeys[keyH] = privKey;
		Storage::pubKeys[keyH] = pubKey;
		Storage::keyCounts[keyH] = keyC;

		lineNumber++;
	}
}

void Storage::save() {
	ofstream file{ Storage::filename };
	Storage::save(file);
}

void Storage::save(ostream& outputStream) {
	for (auto& keypair : Storage::appParams) {
		const auto& keyID = keypair.first;
		const auto& appParam = keypair.second;
		const auto& privKey = Storage::privKeys[keyID];
		const auto& pubKey = Storage::pubKeys[keyID];
		const auto& keyCount = Storage::keyCounts[keyID];

		outputStream << keyID;
		outputStream << ' ';

		string appPStr{};
		b64encode(appParam, appPStr);
		outputStream << appPStr << ' ';

		string privKStr{};
		b64encode(privKey, privKStr);
		outputStream << privKStr << ' ';

		string pubKStr{};
		b64encode(pubKey, pubKStr);
		outputStream << pubKStr << ' ';

		outputStream << keyCount << endl;
	}
}

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

#include "IO.hpp"
#include "Streams.hpp"
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctime>
#include <chrono>
#include <ratio>
#include <array>
#include <string>
#include <stdexcept>
#include <memory>
#include <cstdio>
#include <android/log.h>
#include "u2f.hpp"
#include "Macro.hpp"
#include "U2FDevice.hpp"

using namespace std;

bool bytesAvailable(size_t count);
vector<uint8_t>& getBuffer();

string binaryDirectory{};
string cacheDirectory{ DEBUG_STREAMS };

// Thanks to https://stackoverflow.com/a/478960
vector<uint8_t> execOutput(const string &cmd);
void execInput(const string &cmd, const vector<uint8_t> &stdinData);

vector<uint8_t> readNonBlock(const size_t count)
{
	if (!bytesAvailable(count))
		return vector<uint8_t>{};

	auto &buffer = getBuffer();
	auto buffStart = buffer.begin(), buffEnd = buffer.begin() + count;
	vector<uint8_t> bytes{ buffStart, buffEnd };
	buffer.erase(buffStart, buffEnd);

	errno = 0;

	return bytes;
}

void write(const vector<uint8_t> &bytes)
{
    __android_log_print(ANDROID_LOG_DEBUG, "U2FAndroid", "Writing %zu bytes", bytes.size());
	execInput("su -c \"" + binaryDirectory + "/U2FAndroid_Write\"", bytes);
}

bool bytesAvailable(const size_t count)
{
	auto startTime = std::chrono::high_resolution_clock::now();
	const timespec iterDelay{ 0, 10000000 };
	chrono::duration<double, milli> delay{ 0 };

	while (delay.count() < U2FHID_TRANS_TIMEOUT)
	{
		if (getBuffer().size() >= count) {
#ifdef DEBUG_MSGS
			clog << "Requested " << count << " bytes" << endl;
#endif
			return true;
		}
		nanosleep(&iterDelay, nullptr);
        delay = chrono::high_resolution_clock::now() - startTime;
	}

#ifdef DEBUG_MSGS
	cerr << "Failed to obtain " << count << " bytes, having " << getBuffer().size() << endl;
#endif

	return false;
}

vector<uint8_t>& bufferVar()
{
	static vector<uint8_t> buffer{};
	return buffer;
}

vector<uint8_t>& getBuffer()
{
	auto &buff = bufferVar();
	vector<uint8_t> bytes = execOutput("su -c \"" + binaryDirectory + "/U2FAndroid_Read\"");

	if (!bytes.empty())
	{
		__android_log_print(ANDROID_LOG_DEBUG, "U2FAndroid", "Reading bytes: got %zu", bytes.size());
		buff.insert(buff.end(), bytes.begin(), bytes.end());

#ifdef DEBUG_STREAMS
		fwrite(bytes.data(), 1, bytes.size(), getComHostStream().get());
#endif
	}

	return buff;
}

vector<uint8_t> execOutput(const string &cmd)
{
	// NOLINT(hicpp-member-init)
	array<char, HID_RPT_SIZE> buffer;
	vector<uint8_t> result{};
	unique_ptr<FILE, decltype(&pclose)> pipe{ popen(cmd.c_str(), "rb"), pclose };

	if (!pipe)
		throw std::runtime_error("popen() failed!");
	while (size_t readBytes = fread(buffer.data(), 1, buffer.size(), pipe.get()))
		copy(buffer.begin(), buffer.begin() + readBytes, back_inserter(result));

	return result;
}

void execInput(const string &cmd, const vector<uint8_t> &stdinData)
{
	assert(stdinData.size() % HID_RPT_SIZE == 0);

	size_t writtenBytes = 0;
	unique_ptr<FILE, decltype(&pclose)> pipe{ popen(cmd.c_str(), "wb"), pclose };

	if (!pipe)
		throw std::runtime_error("popen() failed!");
	while (writtenBytes < stdinData.size())
		writtenBytes += fwrite(stdinData.data(), 1, HID_RPT_SIZE, pipe.get());
}

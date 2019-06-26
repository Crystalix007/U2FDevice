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
//#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <chrono>
#include <ratio>
#include "u2f.hpp"
#include "Macro.hpp"
#include "U2FDevice.hpp"

using namespace std;

bool bytesAvailable(const size_t count);
vector<uint8_t>& getBuffer();

vector<uint8_t> readNonBlock(const size_t count)
{
	if (!bytesAvailable(count))
	{
		return vector<uint8_t>{};
	}

	auto &buffer = getBuffer();
	auto buffStart = buffer.begin(), buffEnd = buffer.begin() + count;
	vector<uint8_t> bytes{ buffStart, buffEnd };
	buffer.erase(buffStart, buffEnd);

	errno = 0;

	return bytes;
}

void write(const uint8_t* bytes, const size_t count)
{
	size_t totalBytes = 0;
	auto   hostDescriptor = *getHostDescriptor();

	while (totalBytes < count)
	{
		auto writtenBytes = write(hostDescriptor, bytes + totalBytes, count - totalBytes);

		if (writtenBytes > 0)
			totalBytes += writtenBytes;
		else if (errno != 0 && errno != EAGAIN && errno != EWOULDBLOCK) //Expect file blocking behaviour
			ERR();
	}

	errno = 0;
}

bool bytesAvailable(const size_t count)
{
	auto startTime = std::chrono::high_resolution_clock::now();
	const timespec iterDelay{ 0, 1000 };
	chrono::duration<double, milli> delay{ 0 };

	while (delay.count() < U2FHID_TRANS_TIMEOUT && contProc)
	{
		delay = chrono::high_resolution_clock::now() - startTime;
		if (getBuffer().size() >= count) {
#ifdef DEBUG_MSGS
			clog << "Requested " << count << " bytes" << endl;
#endif
			return true;
		}
		nanosleep(&iterDelay, nullptr);
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
	array<uint8_t, HID_RPT_SIZE> bytes{};
	auto hostDescriptor = *getHostDescriptor();

	while (true)
	{
		auto readByteCount = read(hostDescriptor, bytes.data(), HID_RPT_SIZE);

		if (readByteCount > 0 && readByteCount != HID_RPT_SIZE)
		{
			//Failed to copy an entire packet in, so log this packet
#ifdef DEBUG_MSGS
			cerr << "Only retrieved " << readByteCount << " bytes from expected full packet." << endl;
#endif
		}

		if (readByteCount > 0)
		{
			copy(bytes.begin(), bytes.begin() + readByteCount, back_inserter(buff));

#ifdef DEBUG_STREAMS
			fwrite(bytes.data(), 1, readByteCount, getComHostStream().get());
#endif

		}
		else if (errno != EAGAIN && errno != EWOULDBLOCK) //Expect read would block
		{
			ERR();
#if DEBUG_MSGS
			cerr << "Unknown stream error: " << errno << endl;
#endif
			break;
		}
		else
		{
			errno = 0;
			break; //Escape loop if blocking would occur
		}
	}

	return buff;
}

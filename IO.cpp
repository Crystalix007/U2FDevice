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
#include "Macro.hpp"
#include "Streams.hpp"
#include "U2FDevice.hpp"
#include "u2f.hpp"
#include <android/log.h>
#include <array>
#include <chrono>
#include <cstdio>
#include <ctime>
#include <fcntl.h>
#include <iostream>
#include <memory>
#include <poll.h>
#include <ratio>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

bool bytesAvailable(size_t count);
vector<uint8_t>& getBuffer();

#ifdef DEBUG_STREAMS
string cacheDirectory{ DEBUG_STREAMS };
#endif

#ifdef HID_SOCKET
string clientSocket{};
#endif

vector<uint8_t> readNonBlock(const size_t count) {
	if (!bytesAvailable(count))
		return vector<uint8_t>{};

	auto& buffer = getBuffer();
	auto buffStart = buffer.begin(), buffEnd = buffer.begin() + count;
	vector<uint8_t> bytes{ buffStart, buffEnd };
	buffer.erase(buffStart, buffEnd);

	errno = 0;

	return bytes;
}

void write(const vector<uint8_t>& bytes) {
	size_t totalBytes = 0;
	auto hostDescriptor = *getHostDescriptor();

	while (totalBytes < bytes.size()) {
		auto writtenBytes =
		    send(hostDescriptor, bytes.data() + totalBytes, bytes.size() - totalBytes, 0);

		if (writtenBytes > 0)
			totalBytes += writtenBytes;
		else if (errno != 0 && errno != EAGAIN &&
		         errno != EWOULDBLOCK) // Expect file blocking behaviour
			ERR();
	}

	errno = 0;
}

bool bytesAvailable(const size_t count) {
	auto startTime = std::chrono::high_resolution_clock::now();
	chrono::duration<double, milli> delay{ 0 };

	while (delay.count() < U2FHID_TRANS_TIMEOUT) {
		if (getBuffer().size() >= count) {
#ifdef DEBUG_MSGS
			clog << "Requested " << count << " bytes" << endl;
#endif
			return true;
		}
		delay = chrono::high_resolution_clock::now() - startTime;
	}

#ifdef DEBUG_MSGS
	cerr << "Failed to obtain " << count << " bytes, having " << getBuffer().size() << endl;
#endif

	return false;
}

vector<uint8_t>& bufferVar() {
	static vector<uint8_t> buffer{};
	return buffer;
}

vector<uint8_t>& getBuffer() {
	const timespec delay{ 0, 10'000'000 };

	auto& buff = bufferVar();
	array<uint8_t, HID_RPT_SIZE> bytes{};
	auto hostDescriptor = *getHostDescriptor();

	pollfd pFD{ hostDescriptor, POLLIN };

	while (true) {
		ppoll(&pFD, 1, &delay, 0);

		if (pFD.revents & POLLIN) {
			auto readByteCount = recv(hostDescriptor, bytes.data(), HID_RPT_SIZE, 0);

			if (readByteCount > 0 && readByteCount != HID_RPT_SIZE) {
				// Failed to copy an entire packet in, so log this packet
#ifdef DEBUG_MSGS
				cerr << "Only retrieved " << readByteCount << " bytes from expected full packet."
				     << endl;
#endif
			}

			if (readByteCount > 0) {
				copy(bytes.begin(), bytes.begin() + readByteCount, back_inserter(buff));

#ifdef DEBUG_STREAMS
				fwrite(bytes.data(), 1, readByteCount, getComHostStream().get());
#endif

			} else if (errno != EAGAIN && errno != EWOULDBLOCK) // Expect read would block
			{
				ERR();
#ifdef DEBUG_MSGS
				cerr << "Unknown stream error: " << errno << endl;
#endif
				break;
			} else {
				errno = 0;
				break; // Escape loop if blocking would occur
			}
		} else
			break;
	}

	return buff;
}

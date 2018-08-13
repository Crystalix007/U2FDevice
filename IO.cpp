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
#include <stropts.h>
//#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "u2f.hpp"
#include "Macro.hpp"

using namespace std;

bool bytesAvailable(const size_t count);
vector<uint8_t>& getBuffer();

vector<uint8_t> readNonBlock(const size_t count)
{
	if (!bytesAvailable(count))
	{
		//clog << "No bytes available" << endl;
		return vector<uint8_t>{};
	}

	auto &buffer = getBuffer();
	auto buffStart = buffer.begin(), buffEnd = buffer.begin() + count;
	vector<uint8_t> bytes{ buffStart, buffEnd };
	buffer.erase(buffStart, buffEnd);
	
	fwrite(bytes.data(), 1, bytes.size(), getComHostStream().get());

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
	return getBuffer().size() >= count;
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
	
		if (readByteCount > 0)
		{
			copy(bytes.begin(), bytes.begin() + readByteCount, back_inserter(buff));
		}
		else if (errno != EAGAIN && errno != EWOULDBLOCK) //Expect read would block
		{
			ERR();
		}
		else
		{
			break; //Escape loop if blocking would occur
		}
	}

	return buff;
}


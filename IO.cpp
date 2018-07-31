#include "IO.hpp"
#include "Streams.hpp"
#include <iostream>

using namespace std;

vector<uint8_t> readBytes(const size_t count)
{
	vector<uint8_t> bytes(count);

	size_t readByteCount;
	
	do
	{
		readByteCount = fread(bytes.data(), 1, count, getHostStream().get());
		fwrite(bytes.data(), 1, bytes.size(), getComHostStream().get());
	} while (readByteCount == 0);

	clog << "Read " << readByteCount << " bytes" << endl;

	if (readByteCount != count)
		throw runtime_error{ "Failed to read sufficient bytes" };

	return bytes;
}


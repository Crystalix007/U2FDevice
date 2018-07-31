#include "Streams.hpp"
#include <iostream>

using namespace std;

shared_ptr<FILE> getHostStream()
{
	static shared_ptr<FILE> stream{ fopen("/dev/hidg0", "ab+"), [](FILE *f){
			fclose(f);
	} };

	if (!stream)
		clog << "Stream is unavailable" << endl;

	return stream;
}

shared_ptr<FILE> getComHostStream()
{
	static shared_ptr<FILE> stream{ fopen("comhost.txt", "wb"), [](FILE *f){
		clog << "Closing comhost stream" << endl;
		fclose(f);
	} };

	if (!stream)
		clog << "Stream is unavailable" << endl;

	return stream;
}

shared_ptr<FILE> getHostPacketStream()
{
	static shared_ptr<FILE> stream{ fopen("hostpackets.txt", "wb"), [](FILE *f){
		clog << "Closing hostPackets stream" << endl;
		fclose(f);
	} };

	if (!stream)
		clog << "Stream is unavailable" << endl;

	return stream;
}

shared_ptr<FILE> getComDevStream()
{
	static shared_ptr<FILE> stream{ fopen("comdev.txt", "wb"), [](FILE *f){
		clog << "Closing comdev stream" << endl;	
		fclose(f);
	} };

	if (!stream)
		clog << "Stream is unavailable" << endl;

	return stream;
}

shared_ptr<FILE> getDevPacketStream()
{
	static shared_ptr<FILE> stream{ fopen("devpackets.txt", "wb"), [](FILE *f){
		clog << "Closing devPackets stream" << endl;
		fclose(f);
	} };

	if (!stream)
		clog << "Stream is unavailable" << endl;

	return stream;
}

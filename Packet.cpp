#include "Packet.hpp"
#include "IO.hpp"
#include "u2f.hpp"
#include <cstring>
#include <iostream>
#include "Streams.hpp"

using namespace std;

shared_ptr<InitPacket> InitPacket::getPacket(const uint32_t rCID, const uint8_t rCMD)
{
	auto p        = make_shared<InitPacket>();
	p->cid        = rCID;
	p->cmd        = rCMD;
	p->bcnth      = readBytes(1)[0];
	p->bcntl      = readBytes(1)[0];

	const auto dataBytes = readBytes(p->data.size());
	copy(dataBytes.begin(), dataBytes.end(), p->data.begin());

	auto hPStream = getHostPacketStream().get();
	fprintf(hPStream, "\n");
	fwrite(dataBytes.data(), 1, dataBytes.size(), hPStream);

	clog << "Fully read init packet" << endl;
	return p;
}

shared_ptr<ContPacket> ContPacket::getPacket(const uint32_t rCID, const uint8_t rSeq)
{
	auto p = make_shared<ContPacket>();
	p->cid = rCID;
	p->seq = rSeq;

	const auto dataBytes = readBytes(p->data.size());
	copy(dataBytes.begin(), dataBytes.end(), p->data.begin());

	auto hPStream = getHostPacketStream().get();
	fwrite(dataBytes.data(), 1, dataBytes.size(), hPStream);

	clog << "Fully read cont packet" << endl;
	return p;
}

shared_ptr<Packet> Packet::getPacket()
{
	const uint32_t cid = *reinterpret_cast<uint32_t*>(readBytes(4).data());
	uint8_t b = readBytes(1)[0];

	clog << "Packet read 2nd byte as " << static_cast<uint16_t>(b) << endl;

	if (b & TYPE_MASK)
	{
		//Init packet
		return InitPacket::getPacket(cid, b);
	}
	else
	{
		//Cont packet
		return ContPacket::getPacket(cid, b);
	}
}

void Packet::writePacket()
{
	memset(this->buf, 0,    packetSize);
	memcpy(this->buf, &cid, 4);
}

void InitPacket::writePacket()
{
	Packet::writePacket();
	auto hostStream = getHostStream().get();
	auto devStream  = getComDevStream().get();

	memcpy(this->buf + 4, &cmd,                  1);
	memcpy(this->buf + 5, &bcnth,                1);
	memcpy(this->buf + 6, &bcntl,                1);
	memcpy(this->buf + 7, data.data(), data.size());
	fwrite(this->buf,     packetSize,            1, hostStream);
	fwrite(this->buf,     packetSize,            1,  devStream);

	perror(nullptr);

	auto dPStream = getDevPacketStream().get();
	fprintf(dPStream, "\n");
	fwrite(data.data(), 1, data.size(), dPStream);

	clog << "Fully wrote init packet" << endl;
}

void ContPacket::writePacket()
{
	Packet::writePacket();
	auto hostStream = getHostStream().get();
	auto devStream  = getComDevStream().get();

	memcpy(this->buf + 4, &seq,                  1);
	memcpy(this->buf + 5, data.data(), data.size());
	fwrite(this->buf,      packetSize,           1, hostStream);
	fwrite(this->buf,      packetSize,           1,  devStream);

	perror(nullptr);

	auto dPStream = getDevPacketStream().get();
	fwrite(data.data(), 1, data.size(), dPStream);

	clog << "Fully wrote cont packet" << endl;
}

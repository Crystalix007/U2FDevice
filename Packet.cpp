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
	fprintf(hPStream, "\t\t<table>\n"
			"\t\t\t<thead>\n"
			"\t\t\t\t<tr>\n"
			"\t\t\t\t\t<th>CID</th>\n"
			"\t\t\t\t\t<th>CMD</th>\n"
			"\t\t\t\t\t<th>BCNTH</th>\n"
			"\t\t\t\t\t<th>BCNTL</th>\n"
			"\t\t\t\t\t<th class=\"data\">DATA</th>\n"
			"\t\t\t\t</tr>\n"
			"\t\t\t</thead>\n"
			"\t\t\t<tbody>\n"
			"\t\t\t\t<tr>\n"
			"\t\t\t\t\t<td>0x%08X</td>\n"
			"\t\t\t\t\t<td>%u</td>\n"
			"\t\t\t\t\t<td>%u</td>\n"
			"\t\t\t\t\t<td>%u</td>\n"
			"\t\t\t\t\t<td class=\"data\">", p->cid, p->cmd, p->bcnth, p->bcntl);

	for (auto elem : dataBytes)
		fprintf(hPStream, "%3u ", elem);

	fprintf(hPStream, "</td>\n"
			"\t\t\t\t</tr>\n"
			"\t\t\t</tbody>\n"
			"\t\t</table>"
			"\t\t<br />");

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
	fprintf(hPStream, "\t\t<table>\n"
			"\t\t\t<thead>\n"
			"\t\t\t\t<tr>\n"
			"\t\t\t\t\t<th>CID</th>\n"
			"\t\t\t\t\t<th>SEQ</th>\n"
			"\t\t\t\t\t<th class=\"data\">DATA</th>\n"
			"\t\t\t\t</tr>\n"
			"\t\t\t</thead>\n"
			"\t\t\t<tbody>\n"
			"\t\t\t\t<tr>\n"
			"\t\t\t\t\t<td>0x%08X</td>\n"
			"\t\t\t\t\t<td>%u</td>\n"
			"\t\t\t\t\t<td class=\"data\">", p->cid, p->seq);

	for (auto elem : dataBytes)
		fprintf(hPStream, "%3u ", elem);

	fprintf(hPStream, "</td>\n"
			"\t\t\t\t</tr>\n"
			"\t\t\t</tbody>\n"
			"\t\t</table>\n"
			"\t\t<br />");

	//clog << "Fully read cont packet" << endl;
	return p;
}

shared_ptr<Packet> Packet::getPacket()
{
	const uint32_t cid = *reinterpret_cast<uint32_t*>(readBytes(4).data());
	uint8_t b = readBytes(1)[0];

	//clog << "Packet read 2nd byte as " << static_cast<uint16_t>(b) << endl;

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

	if (errno != 0)
		perror("perror " __FILE__ " 85");

	auto dPStream = getDevPacketStream().get();
	fprintf(dPStream, "\t\t<table>\n"
			"\t\t\t<thead>\n"
			"\t\t\t\t<tr>\n"
			"\t\t\t\t\t<th>CID</th>\n"
			"\t\t\t\t\t<th>CMD</th>\n"
			"\t\t\t\t\t<th>BCNTH</th>\n"
			"\t\t\t\t\t<th>BCNTL</th>\n"
			"\t\t\t\t\t<th class=\"data\">DATA</th>\n"
			"\t\t\t\t</tr>\n"
			"\t\t\t</thead>\n"
			"\t\t\t<tbody>\n"
			"\t\t\t\t<tr>\n"
			"\t\t\t\t\t<td>0x%08X</td>\n"
			"\t\t\t\t\t<td>%u</td>\n"
			"\t\t\t\t\t<td>%u</td>\n"
			"\t\t\t\t\t<td>%u</td>\n"
			"\t\t\t\t\t<td class=\"data\">", cid, cmd, bcnth, bcntl);

	for (auto elem : data)
		fprintf(dPStream, "%3u ", elem);

	fprintf(dPStream, "</td>\n"
			"\t\t\t\t</tr>\n"
			"\t\t\t</tbody>\n"
			"\t\t</table>"
			"\t\t<br />");

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

	if (errno != 0)
		perror("perror " __FILE__ " 107");

	auto dPStream = getDevPacketStream().get();

	fprintf(dPStream, "\t\t<table>\n"
			"\t\t\t<thead>\n"
			"\t\t\t\t<tr>\n"
			"\t\t\t\t\t<th>CID</th>\n"
			"\t\t\t\t\t<th>SEQ</th>\n"
			"\t\t\t\t\t<th class=\"data\">DATA</th>\n"
			"\t\t\t\t</tr>\n"
			"\t\t\t</thead>\n"
			"\t\t\t<tbody>\n"
			"\t\t\t\t<tr>\n"
			"\t\t\t\t\t<td>0x%08X</td>\n"
			"\t\t\t\t\t<td>%u</td>\n"
			"\t\t\t\t\t<td class=\"data\">", cid, seq);

	for (auto elem : data)
		fprintf(dPStream, "%3u ", elem);

	fprintf(dPStream, "</td>\n"
			"\t\t\t\t</tr>\n"
			"\t\t\t</tbody>\n"
			"\t\t</table>\n"
			"\t\t<br />");
}

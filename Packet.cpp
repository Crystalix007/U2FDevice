#include "Packet.hpp"
#include "IO.hpp"
#include "u2f.hpp"
#include <cstring>
#include <iostream>
#include <unistd.h>
#include "Streams.hpp"

using namespace std;

shared_ptr<InitPacket> InitPacket::getPacket(const uint32_t rCID, const uint8_t rCMD)
{
	static size_t bytesRead = 0;
	static uint8_t bcnth;
	static uint8_t bcntl;
	static decltype(InitPacket::data) dataBytes;
	vector<uint8_t> bytes{};

	switch (bytesRead)
	{
		case 0:
			bytes = readNonBlock(1);

			if (bytes.size() == 0)
				return {};

			bcnth = bytes[0];
			bytesRead += bytes.size();
			[[fallthrough]];

		case 1:
			bytes = readNonBlock(1);

			if (bytes.size() == 0)
				return {};

			bcntl = bytes[0];
			bytesRead += bytes.size();
			[[fallthrough]];

		case 2:
			bytes = readNonBlock(dataBytes.size());

			if (bytes.size() == 0)
				return {};

			copy(bytes.begin(), bytes.end(), dataBytes.begin());;
			bytesRead += bytes.size();
			[[fallthrough]];

		case 2 + dataBytes.size():
			break;

		default:
			throw runtime_error{ "Unknown stage in InitPacket construction" };
	}

	auto p = make_shared<InitPacket>();
	p->cid   = rCID;
	p->cmd   = rCMD;
	p->bcnth = bcnth;
	p->bcntl = bcntl;
	p->data  = dataBytes;

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
	bytesRead = 0;
	return p;
}

shared_ptr<ContPacket> ContPacket::getPacket(const uint32_t rCID, const uint8_t rSeq)
{
	static size_t readBytes = 0;
	static decltype(ContPacket::data) dataBytes;

	vector<uint8_t> bytes{};
	auto p = make_shared<ContPacket>();

	if (readBytes != dataBytes.size())
	{
		dataBytes = {};
		bytes = readNonBlock(dataBytes.size());

		if (bytes.size() == 0)
			return {};

		copy(bytes.begin(), bytes.end(), dataBytes.begin());
		readBytes += bytes.size();
	}

	p->cid = rCID;
	p->seq = rSeq;
	p->data = dataBytes;

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
	readBytes = 0;
	return p;
}

shared_ptr<Packet> Packet::getPacket()
{
	static size_t bytesRead = 0;
	vector<uint8_t> bytes{};

	static uint32_t cid;
	static uint8_t  b;
	shared_ptr<Packet> packet{};

	switch (bytesRead)
	{
		case 0:
			bytes = readNonBlock(4);
			
			if (bytes.size() == 0)
				return {};

			cid = *reinterpret_cast<uint32_t*>(bytes.data());
			bytesRead += bytes.size();
			[[fallthrough]];

		case 4:
			bytes = readNonBlock(1);

			if (bytes.size() == 0)
				return {};

			b = bytes[0];
			bytesRead += bytes.size();
			[[fallthrough]];

		case 5:
			if (b & TYPE_MASK)
			{
				//Init packet
				//clog << "Getting init packet" << endl;
				packet = InitPacket::getPacket(cid, b);

				if (packet)
					bytesRead = 0;

				return packet;
			}
			else
			{
				//Cont packet
				//clog << "Getting cont packet" << endl;
				packet = ContPacket::getPacket(cid, b);

				if (packet)
					bytesRead = 0;

				return packet;
			}
		default:
			throw runtime_error{ "Unknown stage in Packet construction" };
	}
}

void Packet::writePacket()
{
	memset(this->buf, 0,    HID_RPT_SIZE);
	memcpy(this->buf, &cid, 4);
}

void InitPacket::writePacket()
{
	Packet::writePacket();
	auto devStream  = getComDevStream().get();

	memcpy(this->buf + 4, &cmd,                  1);
	memcpy(this->buf + 5, &bcnth,                1);
	memcpy(this->buf + 6, &bcntl,                1);
	memcpy(this->buf + 7, data.data(), data.size());
	write(this->buf,      sizeof(this->buf));
	fwrite(this->buf,     1,           sizeof(this->buf), devStream);

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
}

void ContPacket::writePacket()
{
	Packet::writePacket();
	auto devStream  = getComDevStream().get();

	memcpy(this->buf + 4, &seq,                  1);
	memcpy(this->buf + 5, data.data(), data.size());
	write(this->buf,      HID_RPT_SIZE);
	fwrite(this->buf,     HID_RPT_SIZE,           1,  devStream);

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

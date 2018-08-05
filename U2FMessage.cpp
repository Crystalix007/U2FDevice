#include "U2FMessage.hpp"
#include "Packet.hpp"
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include "Streams.hpp"
#include "u2f.hpp"

using namespace std;

U2FMessage U2FMessage::read()
{
	auto fPack = dynamic_pointer_cast<InitPacket>(Packet::getPacket());

	if (!fPack)
		throw runtime_error{ "Failed to receive Init packet" };

	const uint16_t messageSize = ((static_cast<uint16_t>(fPack->bcnth) << 8u) + fPack->bcntl);

	clog << "Message on channel 0x" << hex << fPack->cid << dec << " has size: " << messageSize << endl;

	const uint16_t copyByteCount = min(static_cast<uint16_t>(fPack->data.size()), messageSize);

	U2FMessage message{ fPack-> cid, fPack->cmd };
	message.data.assign(fPack->data.begin(), fPack->data.begin() + copyByteCount);

	uint8_t currSeq = 0;

	while (message.data.size() < messageSize)
	{
		auto newPack = dynamic_pointer_cast<ContPacket>(Packet::getPacket());

		if (!newPack)
			throw runtime_error{ "Failed to receive Cont packet" };
		else if (newPack->seq != currSeq)
			throw runtime_error{ "Packet out of sequence" };

		const uint16_t remainingBytes = messageSize - message.data.size();
		const uint16_t copyBytes = min(static_cast<uint16_t>(newPack->data.size()), remainingBytes);
		message.data.insert(message.data.end(), newPack->data.begin(), newPack->data.begin() + copyBytes);

		currSeq++;
	}

	std::clog << "Read all of message" << std::endl;

	return message;
}

void U2FMessage::write()
{
	clog << "Flushing host stream" << endl;
	fflush(getHostStream().get());
	clog << "Flushed host stream" << endl;
	const uint16_t bytesToWrite = this->data.size();
	uint16_t bytesWritten = 0;

	{
		const uint8_t bcnth = bytesToWrite >> 8;
		const uint8_t bcntl = bytesToWrite - (bcnth << 8);

		InitPacket p{};
		p.cid = cid;
		p.cmd = cmd;
		p.bcnth = bcnth;
		p.bcntl = bcntl;

		{
			uint16_t initialByteCount = min(static_cast<uint16_t>(p.data.size()), static_cast<uint16_t>(bytesToWrite - bytesWritten));
			copy(data.begin(), data.begin() + initialByteCount, p.data.begin());
			bytesWritten += initialByteCount;
		}

		p.writePacket();
	}

	uint8_t seq = 0;

	while (bytesWritten != bytesToWrite)
	{
		ContPacket p{};
		p.cid = cid;
		p.seq = seq;
		uint16_t newByteCount = min(static_cast<uint16_t>(p.data.size()), static_cast<uint16_t>(bytesToWrite - bytesWritten));
		copy(data.begin() + bytesWritten, data.begin() + bytesWritten + newByteCount, p.data.begin());
		p.writePacket();
		seq++;
		bytesWritten += newByteCount;
	}

	auto stream = getHostStream().get();
	fflush(stream);

	if (cmd == U2FHID_MSG)
	{
		auto dAS = getDevAPDUStream().get();
	
		fprintf(dAS, "<table>\n"
				"\t\t\t<thead>\n"
				"\t\t\t\t<tr>\n"
				"\t\t\t\t\t<th>DATA</th>\n"
				"\t\t\t\t\t<th>ERR</th>\n"
				"\t\t\t\t</tr>\n"
				"\t\t\t</thead>\n"
				"\t\t\t<tbody>\n"
				"\t\t\t\t<tr>\n"
				"\t\t\t\t\t<td class=\"data\">");
		
		for (size_t i = 0; i < data.size() - 2; i++)
			fprintf(dAS, "%3u ", data[i]);
	
		uint16_t err = data[data.size() - 2] << 8;
		err |= data.back();
	
		fprintf(dAS, "</td>\n"
				"\t\t\t\t\t<td>0x%04X</td>\n"
				"\t\t\t\t</tr>\n"
				"\t\t\t</tbody>\n"
				"\t\t</table>\n"
				"\t\t<br />", err);
	}
}

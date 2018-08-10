#include "U2FMessage.hpp"
#include "Packet.hpp"
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include "Streams.hpp"
#include "u2f.hpp"

using namespace std;

shared_ptr<U2FMessage> U2FMessage::readNonBlock()
{
	static size_t	       currSeq = -1;
	static uint16_t        messageSize;
	static uint32_t        cid;
	static uint8_t         cmd;
	static vector<uint8_t> dataBytes;

	shared_ptr<Packet> p{};

	if (currSeq == -1u)
	{
		cid = 0;
		cmd = 0;
		messageSize = 0;
		dataBytes = {};
		
		shared_ptr<InitPacket> initPack{};
		do
		{
			p = Packet::getPacket();

			if (!p)
				return {};

			initPack = dynamic_pointer_cast<InitPacket>(p);
		} while (!initPack); //Spurious cont. packet - spec states ignore

		messageSize = ((static_cast<uint16_t>(initPack->bcnth) << 8u) + initPack->bcntl);
		const uint16_t copyByteCount = min(static_cast<uint16_t>(initPack->data.size()), messageSize);

		cid = initPack->cid;
		cmd = initPack->cmd;
		
		copy(initPack->data.begin(), initPack->data.begin() + copyByteCount, back_inserter(dataBytes));
		currSeq++;
	}

	while (messageSize > dataBytes.size() && static_cast<bool>(p = Packet::getPacket())) //While there is a packet
	{
		auto contPack = dynamic_pointer_cast<ContPacket>(p);

		if (!contPack) //Spurious init. packet
		{
			currSeq = -1; //Reset
			return {};
		}

		if (contPack->cid != cid) //Cont. packet of different CID
		{
			cerr << "Invalid CID: was handling channel 0x" << hex << cid << " and received packet from channel 0x" << contPack->cid << dec << endl;
			U2FMessage::error(contPack->cid, ERR_CHANNEL_BUSY);
			currSeq = -1;
			return {};
		}

		if (contPack->seq != currSeq)
		{
			cerr << "Invalid packet seq. value" << endl;
			U2FMessage::error(cid, ERR_INVALID_SEQ);
			currSeq = -1;
			return {};
		}

		const uint16_t remainingBytes = messageSize - dataBytes.size();
		const uint16_t copyBytes = min(static_cast<uint16_t>(contPack->data.size()), remainingBytes);
		dataBytes.insert(dataBytes.end(), contPack->data.begin(), contPack->data.begin() + copyBytes);
		currSeq++;
	}

	if (messageSize != dataBytes.size())
		return {};

	auto message = make_shared<U2FMessage>(cid, cmd);
	message->data.assign(dataBytes.begin(), dataBytes.end());
	currSeq = -1u;

	return message;
}

void U2FMessage::write()
{
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

U2FMessage::U2FMessage(const uint32_t nCID, const uint8_t nCMD)
	: cid{ nCID }, cmd{ nCMD }
{}

void U2FMessage::error(const uint32_t tCID, const uint16_t tErr)
{
	U2FMessage msg{};
	msg.cid = tCID;
	msg.cmd = U2FHID_ERROR;
	msg.data.push_back((tErr >> 8) & 0xFF);
	msg.data.push_back(tErr & 0xFF);
}

#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>
#include <vector>
#include <algorithm>
#include <iostream>
#include "u2f.hpp"

using namespace std;

const constexpr uint16_t packetSize = 32;

struct Packet
{
	uint32_t                         cid;

	protected:
		Packet() = default;
		virtual void writePacket();

	public:
		static shared_ptr<Packet> getPacket();
		virtual ~Packet() = default;
};


struct InitPacket : Packet
{
	uint8_t                        cmd;
	uint8_t                        bcnth;
	uint8_t                        bcntl;
	array<uint8_t, packetSize - 7> data{};

	public:
		InitPacket() = default;
		static shared_ptr<InitPacket> getPacket(const uint32_t rCID, const uint8_t rCMD);
		void writePacket() override;
};

struct ContPacket : Packet
{
	uint8_t                         seq;
	array<uint8_t, packetSize - 7> data{};

	public:
		ContPacket() = default;
		static shared_ptr<ContPacket> getPacket(const uint32_t rCID, const uint8_t rSeq);
		void writePacket() override;
};

shared_ptr<FILE> getStream()
{
	static shared_ptr<FILE> stream{ fopen("/dev/hidg0", "rwb"), [](FILE *f){
			fclose(f);
	} };

	if (!stream)
		clog << "Stream is unavailable" << endl;

	return stream;
}

vector<uint8_t> readBytes(const size_t count)
{
	vector<uint8_t> bytes(count);

	const auto readByteCount = fread(bytes.data(), 1, count, getStream().get());

	clog << "Read " << readByteCount << " bytes" << endl;

	if (readByteCount != count)
		throw runtime_error{ "Failed to read sufficient bytes" };

	return bytes;
}

shared_ptr<InitPacket> InitPacket::getPacket(const uint32_t rCID, const uint8_t rCMD)
{
	auto p        = make_shared<InitPacket>();
	p->cid        = rCID;
	p->cmd        = rCMD;
	p->bcnth      = readBytes(1)[0];
	p->bcntl      = readBytes(1)[0];
	/*uint16_t pLen =   p->bcnth;
	p->bcnth      <<= 8;
	p->bcnth      +=  p->bcntl;
	*/

	const auto dataBytes = readBytes(p->data.size());
	copy(dataBytes.begin(), dataBytes.end(), p->data.data());

	return p;
}

shared_ptr<ContPacket> ContPacket::getPacket(const uint32_t rCID, const uint8_t rSeq)
{
	auto p = make_shared<ContPacket>();
	p->cid = rCID;
	p->seq = rSeq;

	const auto dataBytes = readBytes(p->data.size());
	copy(dataBytes.begin(), dataBytes.end(), p->data.data());

	return p;
}

shared_ptr<Packet> Packet::getPacket()
{
	clog << "Making generic packet" << endl;
	const uint32_t cid = *reinterpret_cast<uint32_t*>(readBytes(4).data());
	clog << "Grabbed cid" << endl;
	uint8_t b = readBytes(1)[0];

	clog << "b: " << static_cast<uint16_t>(b) << endl;

	if (b && TYPE_MASK)
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
	fwrite(&cid, 4, 1, getStream().get());
}

void InitPacket::writePacket()
{
	Packet::writePacket();
	auto stream = getStream().get();

	fwrite(&cmd,        1,           1, stream);
	fwrite(&bcnth,      1,           1, stream);
	fwrite(&bcntl,      1,           1, stream);
	fwrite(data.data(), data.size(), 1, stream);
}

void ContPacket::writePacket()
{
	Packet::writePacket();
	auto stream = getStream().get();

	fwrite(&seq,        1,           1, stream);
	fwrite(data.data(), data.size(), 1, stream);
}

struct U2FMessage
{
	uint32_t        cid;
	uint8_t         cmd;
	vector<uint8_t> data;

	static U2FMessage read()
	{
		auto fPack = dynamic_pointer_cast<InitPacket>(Packet::getPacket());
	
		if (!fPack)
			throw runtime_error{ "Failed to receive Init packet" };
	
		const uint16_t messageSize = ((static_cast<uint16_t>(fPack->bcnth) << 8u) + fPack->bcntl);
	
		clog << "Message has size: " << messageSize << endl;
	
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
	
		return message;
	}
	
	void write()
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
		}
	}
};

struct U2F_CMD
{
	protected:
		U2F_CMD() = default;

	public:
		~U2F_CMD() = default;
}; //For polymorphic type casting

struct U2F_Init_CMD : U2F_CMD
{
	uint64_t nonce;

	public:
		static U2F_Init_CMD get()
		{
			const auto message = U2FMessage::read();

			if (message.cmd != U2FHID_INIT)
				throw runtime_error{ "Failed to get U2F Init message" };
			else if (message.data.size() != INIT_NONCE_SIZE)
				throw runtime_error{ "Init nonce is incorrect size" };

			U2F_Init_CMD cmd;
			cmd.nonce = *reinterpret_cast<const uint64_t*>(message.data.data());

			return cmd;
		}
};

#define FIELD(name) reinterpret_cast<uint8_t*>(&name), (reinterpret_cast<uint8_t*>(&name) + sizeof(name))

struct U2F_Init_Response : U2F_CMD
{
	uint32_t cid;
	uint64_t nonce;
	uint8_t  protocolVer;
	uint8_t  majorDevVer;
	uint8_t  minorDevVer;
	uint8_t  buildDevVer;
	uint8_t  capabilities;

	void write()
	{
		U2FMessage m{};
		m.cid = CID_BROADCAST;
		m.cmd = U2FHID_INIT;

		m.data.insert(m.data.begin() +  0, FIELD(nonce));
		m.data.insert(m.data.begin() +  8, FIELD(cid));
		m.data.insert(m.data.begin() + 12, FIELD(protocolVer));
		m.data.insert(m.data.begin() + 13, FIELD(majorDevVer));
		m.data.insert(m.data.begin() + 14, FIELD(minorDevVer));
		m.data.insert(m.data.begin() + 15, FIELD(buildDevVer));
		m.data.insert(m.data.begin() + 16, FIELD(capabilities));
	}
};


int main()
{
	auto initFrame = U2F_Init_CMD::get();
	U2F_Init_Response resp{};

	resp.cid          = 0xF1D0F1D0;
	resp.nonce        = initFrame.nonce;
	resp.protocolVer  = 2;
	resp.majorDevVer  = 0;
	resp.minorDevVer  = 0;
	resp.buildDevVer  = 0;
	resp.capabilities = CAPFLAG_WINK;

	resp.write();
	U2FMessage m = m.read();

	for (const auto d : m.data)
		cout << static_cast<uint16_t>(d) << endl;
}

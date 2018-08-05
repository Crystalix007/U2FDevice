#include <cstdint>
#include "U2FMessage.hpp"
#include "Field.hpp"

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
		std::clog << "Beginning writeout of U2F_Init_Response" << std::endl;
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

		std::clog << "Finished inserting U2F_Init_Response fields to data buffer" << std::endl;

		m.write();
		std::clog << "Completed writeout of U2F_Init_Response" << std::endl;
	}
};

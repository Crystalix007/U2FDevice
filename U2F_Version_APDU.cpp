#include "U2F_Version_APDU.hpp"
#include <iostream>
#include "U2FMessage.hpp"
#include "u2f.hpp"
#include "Field.hpp"
#include "APDU.hpp"

using namespace std;

U2F_Version_APDU::U2F_Version_APDU(const U2F_Msg_CMD &msg)
{
	//Don't actually respond yet unless invalid
	if (msg.p1 != 0 || msg.p2 != 0)
		throw APDU_STATUS::SW_COMMAND_NOT_ALLOWED;
	else if (msg.data.size() != 0)
		throw APDU_STATUS::SW_WRONG_LENGTH;
}

void U2F_Version_APDU::respond(const uint32_t channelID) const
{
	char ver[]{ 'U', '2', 'F', '_', 'V', '2' };
	U2FMessage m{};

	m.cid = channelID;
	m.cmd = U2FHID_MSG;
	m.data.insert(m.data.end(), FIELD(ver));
	auto sC = APDU_STATUS::SW_NO_ERROR;
	m.data.insert(m.data.end(), FIELD_BE(sC));
	m.write();
}

#include "U2F_Version_APDU.hpp"
#include <iostream>
#include "U2FMessage.hpp"
#include "u2f.hpp"
#include "Field.hpp"
#include "APDU.hpp"

using namespace std;

U2F_Version_APDU::U2F_Version_APDU(const U2F_Msg_CMD &msg)
{
	clog << "Got U2F_Ver APDU request" << endl;

	//Don't actually respond yet
}

void U2F_Version_APDU::respond()
{
	char ver[]{ 'U', '2', 'F', '_', 'V', '2' };
	U2FMessage m{};

	m.cid = 0xF1D0F1D0;
	m.cmd = U2FHID_MSG;
	m.data.insert(m.data.end(), FIELD(ver));
	auto sC = APDU_STATUS::SW_NO_ERROR;
	m.data.insert(m.data.end(), FIELD_BE(sC));
	m.write();
}

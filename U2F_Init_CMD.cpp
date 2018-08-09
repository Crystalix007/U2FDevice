#include "U2F_Init_CMD.hpp"
#include <stdexcept>
#include "u2f.hpp"
#include "Field.hpp"

using namespace std;

U2F_Init_CMD::U2F_Init_CMD(const shared_ptr<U2FMessage> uMsg)
{
	if (uMsg->cmd != U2FHID_INIT)
		throw runtime_error{ "Failed to get U2F Init message" };
	else if (uMsg->data.size() != INIT_NONCE_SIZE)
		throw runtime_error{ "Init nonce is incorrect size" };

	this->nonce = *reinterpret_cast<const uint64_t*>(uMsg->data.data());
}

void U2F_Init_CMD::respond(const uint32_t channelID) const
{
	U2FMessage msg{};
	msg.cid = CID_BROADCAST;
	msg.cmd = U2FHID_INIT;

	msg.data.insert(msg.data.end(), FIELD(this->nonce));
	msg.data.insert(msg.data.end(), FIELD(channelID));
	msg.data.push_back(2); //Protocol version
	msg.data.push_back(1); //Major device version
	msg.data.push_back(0); //Minor device version
	msg.data.push_back(1); //Build device version
	msg.data.push_back(CAPFLAG_WINK); //Wink capability

	msg.write();
}

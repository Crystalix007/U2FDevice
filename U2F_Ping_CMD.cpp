#include "U2F_Ping_CMD.hpp"
#include "u2f.hpp"

using namespace std;

U2F_Ping_CMD::U2F_Ping_CMD(const shared_ptr<U2FMessage> uMsg)
	: nonce{ uMsg->data }
{
	if (uMsg->cmd != U2FHID_PING)
		throw runtime_error{ "Failed to get U2F ping message" };
}

void U2F_Ping_CMD::respond(const uint32_t channelID) const
{
	U2FMessage msg{};
	msg.cid = channelID;
	msg.cmd = U2FHID_PING;
	msg.data = nonce;
	msg.write();
}

#include "U2F_Init_CMD.hpp"
#include <stdexcept>
#include "U2FMessage.hpp"
#include "u2f.hpp"

using namespace std;

U2F_Init_CMD U2F_Init_CMD::get()
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

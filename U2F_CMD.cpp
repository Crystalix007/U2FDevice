#include "U2F_CMD.hpp"
#include "u2f.hpp"
#include "U2F_Msg_CMD.hpp"
#include "U2F_Init_CMD.hpp"
#include "U2F_Ping_CMD.hpp"

using namespace std;

shared_ptr<U2F_CMD> U2F_CMD::get(const shared_ptr<U2FMessage> uMsg)
{
	try
	{
		switch (uMsg->cmd)
		{
			case U2FHID_PING:
				return make_shared<U2F_Ping_CMD>(uMsg);
			case U2FHID_MSG:
					return U2F_Msg_CMD::generate(uMsg);
			case U2FHID_INIT:
				return make_shared<U2F_Init_CMD>(uMsg);
			default:
				return {};
		}
	}
	catch (runtime_error)
	{
		return {};
	}
}

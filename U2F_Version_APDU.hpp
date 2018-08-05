#pragma once
#include "U2F_Msg_CMD.hpp"

struct U2F_Version_APDU : U2F_Msg_CMD
{
	public:
		U2F_Version_APDU(const U2F_Msg_CMD &msg);
		void respond();
};

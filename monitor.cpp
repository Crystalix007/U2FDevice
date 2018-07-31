#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>
#include <vector>
#include <algorithm>
#include <iostream>
#include <cstring>
#include <map>
#include "u2f.hpp"
#include "Constants.hpp"
#include "U2FMessage.hpp"
#include "U2F_CMD.hpp"
#include "Storage.hpp"
#include "U2F_Init_CMD.hpp"
#include "U2F_Init_Response.hpp"
#include "U2F_Msg_CMD.hpp"

using namespace std;

int main()
{
	Storage::init();

	auto initFrame = U2F_Init_CMD::get();
	U2F_Init_Response resp{};

	resp.cid          = 0xF1D0F1D0;
	resp.nonce        = initFrame.nonce;
	resp.protocolVer  = 2;
	resp.majorDevVer  = 1;
	resp.minorDevVer  = 0;
	resp.buildDevVer  = 1;
	resp.capabilities = CAPFLAG_WINK;

	resp.write();

	auto m = U2F_Msg_CMD::get();

	cout << "U2F CMD ins: " << static_cast<uint32_t>(m->ins) << endl;

	/*auto m = U2FMessage::read();

	cout << "U2F CMD: " << static_cast<uint32_t>(m.cmd) << endl;

	for (const auto d : m.data)
		clog << static_cast<uint16_t>(d) << endl;*/

	Storage::save();
}

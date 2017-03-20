#pragma once

#include "LoginPacket.h"

namespace RakLib {
	class Reply2 : public LoginPacket {
	public:
		int64 serverID;
		uint16 port;
		int16 mtuSize;
		bool security;

	public:
		Reply2(int64 serverID, uint16 port, int16 mtu, bool security);

		void encode();

	};
}
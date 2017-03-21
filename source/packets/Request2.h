#pragma once

#include "LoginPacket.h"

namespace RakLib {
	class Request2 : public LoginPacket {
	public:
		bool security;
		int32 cookie;
		uint16 port;
		uint16 mtuSize;
		int64 clientID;

	public:
		Request2(std::unique_ptr<Packet> packet);

		void decode();
	};
}
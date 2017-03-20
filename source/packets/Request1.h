#pragma once

#include "LoginPacket.h"

namespace RakLib {
	class Request1 : public LoginPacket {
	public:
		uint8 version;
		short mtuSize;

	public:
		Request1(Packet* packet);

		void decode();
	};
}
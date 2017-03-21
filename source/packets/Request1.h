#pragma once

#include "LoginPacket.h"

namespace RakLib {
	class Request1 : public LoginPacket {
	public:
		uint8 version;
		int16 mtuSize;

	public:
		Request1(std::unique_ptr<Packet> packet);

		void decode();
	};
}
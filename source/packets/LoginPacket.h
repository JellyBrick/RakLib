#pragma once

#include <memory>

#include "../network/Packet.h"
#include "Info.h"


namespace RakLib {
	class LoginPacket : public Packet {
	public:
		LoginPacket();
		LoginPacket(std::unique_ptr<Packet> pck);
		LoginPacket(uint32 size);

		virtual void decode();
		virtual void encode();

		uint8* getMagic();
	};
}

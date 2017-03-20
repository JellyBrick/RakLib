#pragma once

#include "InternalPacket.h"
#include "../network/Packet.h"

namespace RakLib {
	class DataPacket : public Packet {
	public:
		DataPacket(InternalPacket* pck) : Packet(pck->buff, pck->length) {}

		DataPacket(Packet* packet) : Packet(packet) {}

		DataPacket(uint32 size) : Packet(size) {}

		virtual void encode() { };
		virtual void decode() { };
	};
}
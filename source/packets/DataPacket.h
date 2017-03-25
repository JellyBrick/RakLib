#pragma once

#include "InternalPacket.h"
#include "../network/Packet.h"

namespace RakLib {
	class DataPacket : public Packet {
	public:
		DataPacket(InternalPacket* pck) : Packet(pck->buff, pck->length) {}

		DataPacket(std::unique_ptr<Packet> packet) : Packet(std::move(packet)) {}

		DataPacket(uint32 size) : Packet(size) {}

		DataPacket() : Packet() {}

		virtual void encode() { };
		virtual void decode() { };
	};
}
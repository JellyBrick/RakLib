#pragma once

#include <memory>

#include "../network/Packet.h"

namespace RakLib {
	class DataPacket : public Packet {
	public:
		DataPacket(uint32 size) : Packet(size) {}

		DataPacket(std::unique_ptr<Packet>&& packet) : Packet(packet->getBuffer(), packet->getLength(), "", 0) {
			packet->release();
		}

		virtual void encode() {}

		virtual void decode() {}
	};
}
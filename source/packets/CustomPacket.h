#pragma once

#include <vector>

#include "InternalPacket.h"

namespace RakLib {
	class CustomPacket : public Packet {
	public:
		uint8 packetID;
		uint32 sequenceNumber;
		std::vector<InternalPacket*> packets;

	public:
		CustomPacket(Packet* packet);
		CustomPacket(uint8* data, uint32 size);
		CustomPacket();
		virtual ~CustomPacket();

		uint32 getTotalLength();

		void decode();

		void encode();

	};
}
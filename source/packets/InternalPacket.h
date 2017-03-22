#pragma once

#include <vector>

#include "../network/Packet.h"

namespace RakLib {
	class InternalPacket {
	public:
		uint8* buff;
		uint32 length;
		uint8 reliability = 0;
		bool hasSplit = false;
		uint32 messageIndex;
		uint32 orderIndex;
		uint8 orderChannel;
		uint32 splitCount;
		uint16 splitID;
		uint32 splitIndex;

	public:
		static std::vector<InternalPacket*> fromBinary(Packet& packet);

		uint32 getLength() const;
		std::unique_ptr<Packet> toBinary();
		void close();
	};

}
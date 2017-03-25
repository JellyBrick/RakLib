#pragma once

#include <vector>

#include "../network/Packet.h"

namespace RakLib {
	class InternalPacket {
	public:
		/*
		* From http://www.jenkinssoftware.com/raknet/manual/reliabilitytypes.html
		* Default: 0b010 (2) || 0b011 (3)
		*/
		enum Reliability {
			UNRELIABLE,
			UNRELIABLE_SEQUENCED,
			RELIABLE,
			RELIABLE_ORDERED,
			RELIABLE_SEQUENCED,
			UNRELIABLE_WITH_ACK_RECEIPT,
			RELIABLE_WITH_ACK_RECEIPT,
			RELIABLE_ORDERED_WITH_ACK_RECEIPT,
		};

		uint8* buff;
		uint16 length;
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
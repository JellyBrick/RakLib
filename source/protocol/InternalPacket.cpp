#include "RakLib/protocol/InternalPacket.h"

namespace RakLib {
	std::vector<InternalPacket*> InternalPacket::fromBinary(Packet& packet) {
		std::vector<InternalPacket*> packets;
		while (packet.getPosition() < packet.getLength()) {
			InternalPacket* internalPacket = new InternalPacket();
			uint8 flags = packet.getByte();
			internalPacket->reliability = (flags & 0xE0) >> 5;
			internalPacket->hasSplit = (flags & 0x10) > 0;

			uint16 s = packet.getUShort();
			internalPacket->length = static_cast<uint16>((s + 7) >> 3);

		
			if (internalPacket->reliability == RELIABLE || internalPacket->reliability == RELIABLE_ORDERED || internalPacket->reliability == RELIABLE_SEQUENCED || internalPacket->reliability == RELIABLE_WITH_ACK_RECEIPT || internalPacket->reliability == RELIABLE_ORDERED_WITH_ACK_RECEIPT) {
				internalPacket->messageIndex = packet.getLTriad();
			}

			if (internalPacket->reliability == UNRELIABLE_SEQUENCED || internalPacket->reliability == RELIABLE_ORDERED || internalPacket->reliability == RELIABLE_SEQUENCED || internalPacket->reliability == RELIABLE_ORDERED_WITH_ACK_RECEIPT) {
				internalPacket->orderIndex = packet.getLTriad();
				internalPacket->orderChannel = packet.getByte();
			}

			if (internalPacket->hasSplit) {
				internalPacket->splitCount = packet.getInt();
				internalPacket->splitID = packet.getShort();
				internalPacket->splitIndex = packet.getInt();
			}

			internalPacket->buff = packet.getByte(internalPacket->length);
			packets.push_back(internalPacket);
		}
		return packets;
	}


	std::unique_ptr<Packet> InternalPacket::toBinary(){
		auto packet = std::make_unique<Packet>(getLength());
		packet->putByte(static_cast<uint8>(reliability << 5 | (hasSplit ? 0x01 : 0x00)));
		packet->putUShort(static_cast<uint16>(length << 3));

		if (reliability == RELIABLE || reliability == RELIABLE_ORDERED || reliability == RELIABLE_SEQUENCED || reliability == RELIABLE_WITH_ACK_RECEIPT || reliability == RELIABLE_ORDERED_WITH_ACK_RECEIPT) {
			packet->putLTriad(messageIndex);
		}

		if (reliability == UNRELIABLE_SEQUENCED || reliability == RELIABLE_ORDERED || reliability == RELIABLE_SEQUENCED || reliability == RELIABLE_ORDERED_WITH_ACK_RECEIPT) {
			packet->putLTriad(orderIndex);
			packet->putByte(orderChannel);
		}

		if (hasSplit) {
			packet->putInt(splitCount);
			packet->putShort(splitID);
			packet->putInt(splitIndex);
		}

		packet->putByte(buff, length);
		return std::move(packet);
	}

	uint32 InternalPacket::getLength() const {
		return 3 + length + (messageIndex != -1 ? 3 : 0) + (orderIndex != -1 ? 4 : 0) + (hasSplit ? 10 : 0);
	}

	void InternalPacket::close() {
		if (buff != nullptr) {
			delete[] buff;
			length = 0;
		}
	}
}
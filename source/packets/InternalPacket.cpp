#include "InternalPacket.h"

namespace RakLib {
	std::vector<InternalPacket*> InternalPacket::fromBinary(Packet& packet) {
		std::vector<InternalPacket*> packets;
		while (packet.getPosition() < packet.getLength()) {
			InternalPacket* internalPacket = new InternalPacket();
			uint8 flags = packet.getByte();
			internalPacket->reliability = (flags & 0xE0) >> 5;
			internalPacket->hasSplit = (flags & 0x10) > 0;

			uint16 s = packet.getUShort();
			internalPacket->length = (uint16)(s + 7) >> 3;

		
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
		packet->putByte((uint8)(this->reliability << 5 | (this->hasSplit ? 0x01 : 0x00)));
		packet->putUShort((uint16)(this->length << 3));

		if (this->reliability == RELIABLE || this->reliability == RELIABLE_ORDERED || this->reliability == RELIABLE_SEQUENCED || this->reliability == RELIABLE_WITH_ACK_RECEIPT || this->reliability == RELIABLE_ORDERED_WITH_ACK_RECEIPT) {
			packet->putLTriad(this->messageIndex);
		}

		if (this->reliability == UNRELIABLE_SEQUENCED || this->reliability == RELIABLE_ORDERED || this->reliability == RELIABLE_SEQUENCED || this->reliability == RELIABLE_ORDERED_WITH_ACK_RECEIPT) {
			packet->putLTriad(this->orderIndex);
			packet->putByte(this->orderChannel);
		}

		if (this->hasSplit) {
			packet->putInt(this->splitCount);
			packet->putShort(this->splitID);
			packet->putInt(this->splitIndex);
		}

		packet->putByte(this->buff, this->length);
		return std::move(packet);
	}

	uint32 InternalPacket::getLength() const {
		return 3 + this->length + (this->messageIndex != -1 ? 3 : 0) + (this->orderIndex != -1 ? 4 : 0) + (this->hasSplit ? 10 : 0);
	}


	void InternalPacket::close() {
		delete[] this->buff;
		this->length = 0;
	}
}
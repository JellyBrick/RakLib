#include "InternalPacket.h"

namespace RakLib {
	std::vector<InternalPacket*> InternalPacket::fromBinary(Packet& packet) {
		std::vector<InternalPacket*> packets;
		packet.print();
		while (packet.getPosition() < packet.getLength()) {
			InternalPacket* internalPacket = new InternalPacket();
			uint8 flags = packet.getByte();
			internalPacket->reliability = (flags & 0xE0) >> 5;
			internalPacket->hasSplit = (flags & 0x10) > 0;


			uint16 s = packet.getUShort();
			internalPacket->length = (uint16)(s + 7) >> 3;

			/*
			 * From http://www.jenkinssoftware.com/raknet/manual/reliabilitytypes.html
			 *
			 * Default: 0b010 (2) || 0b011 (3)
			 *
			 * 0: UNRELIABLE
			 * 1: UNRELIABLE_SEQUENCED
			 * 2: RELIABLE
			 * 3: RELIABLE_ORDERED
			 * 4: RELIABLE_SEQUENCED
			 * 5: UNRELIABLE_WITH_ACK_RECEIPT
			 * 6: RELIABLE_WITH_ACK_RECEIPT
			 * 7: RELIABLE_ORDERED_WITH_ACK_RECEIPT
			 */
			if (internalPacket->reliability == 2 || internalPacket->reliability == 3 || internalPacket->reliability == 4 || internalPacket->reliability == 6 || internalPacket->reliability == 7) {
				internalPacket->messageIndex = packet.getLTriad();
			}

			if (internalPacket->reliability == 1 || internalPacket->reliability == 3 || internalPacket->reliability == 4 || internalPacket->reliability == 7) {
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
		packet->putShort((int16)(this->length << 3));

		if (this->reliability == 0x02 || this->reliability == 0x03 || this->reliability == 0x04 || this->reliability == 0x06 || this->reliability == 0x07) {
			packet->putLTriad(this->messageIndex);
		}

		if (this->reliability == 0x01 || this->reliability == 0x03 || this->reliability == 0x04 || this->reliability == 0x07) {
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
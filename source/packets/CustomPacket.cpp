#include "CustomPacket.h"

#include <memory>

namespace RakLib {
	CustomPacket::CustomPacket(std::unique_ptr<Packet>&& packet) : Packet(packet->getBuffer(), packet->getLength(), "", 0), packetID(0x84), sequenceNumber(0) {
		packet->release();
	}

	CustomPacket::CustomPacket(uint32 size) : Packet(size), packetID(0x84), sequenceNumber(0) {}

	CustomPacket::CustomPacket() : packetID(0x84), sequenceNumber(0) {}

	CustomPacket::~CustomPacket() {
		for (const auto& internalPacket : this->packets) {
			internalPacket->close();
			delete internalPacket;
		}

		this->packets.clear();
	}

	uint32 CustomPacket::getTotalLength() {
		uint32 totalLength = 4; // PacketID + sequence number(int24)
		for (const auto& internalPacket : this->packets) {
			totalLength += internalPacket->getLength();
		}
		return totalLength;
	}

	void CustomPacket::decode() {
		this->packetID = this->getByte();
		this->sequenceNumber = (uint32)this->getTriad();
		this->packets = InternalPacket::fromBinary(*this);
	}

	void CustomPacket::encode() {
		this->length = this->getTotalLength();
		this->buffer = new uint8[this->length];

		this->putByte(this->packetID);
		this->putLTriad((int24)this->sequenceNumber);
		for (const auto& internalPacket : packets) {
			std::unique_ptr<Packet> packet = internalPacket->toBinary();
			this->putByte(packet->getBuffer(), packet->getLength());
		}
	}
}
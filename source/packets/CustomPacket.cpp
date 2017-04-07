#include "CustomPacket.h"

#include <memory>

namespace RakLib {
	CustomPacket::CustomPacket(std::unique_ptr<Packet>&& packet) : Packet(packet->getBuffer(), packet->getLength(), "", 0), packetID(0x84), sequenceNumber(0) {
		packet->release();
	}

	CustomPacket::CustomPacket(uint32 size) : Packet(size), packetID(0x84), sequenceNumber(0) {}

	CustomPacket::CustomPacket() : packetID(0x84), sequenceNumber(0) {}

	CustomPacket::~CustomPacket() {
		for (const auto& internalPacket : packets) {
			internalPacket->close();
			delete internalPacket;
		}

		packets.clear();
	}

	uint32 CustomPacket::getTotalLength() {
		uint32 totalLength = 4; // PacketID + sequence number(int24)
		for (const auto& internalPacket : packets) {
			totalLength += internalPacket->getLength();
		}
		return totalLength;
	}

	void CustomPacket::decode() {
		packetID = getByte();
		sequenceNumber = static_cast<uint32>(getTriad());
		packets = InternalPacket::fromBinary(*this);
	}

	void CustomPacket::encode() {
		length = getTotalLength();
		buffer = new uint8[length];

		putByte(packetID);
		putLTriad(static_cast<int24>(sequenceNumber));
		for (const auto& internalPacket : packets) {
			std::unique_ptr<Packet> packet = internalPacket->toBinary();
			putByte(packet->getBuffer(), packet->getLength());
		}
	}
}
#include "CustomPacket.h"

namespace RakLib {
	CustomPacket::CustomPacket(Packet* packet) : Packet(packet) {}

	CustomPacket::CustomPacket(uint8* data, uint32 size) : Packet(data, size) {}

	CustomPacket::CustomPacket() {
		this->packetID = 0x84;
	}

	CustomPacket::~CustomPacket() {
		for (InternalPacket* pck : this->packets) {
			pck->close();
			delete pck;
		}

		this->packets.clear();
	}

	uint32 CustomPacket::getTotalLength() {
		uint32 length = 4; // PacketID + sequence number(int24)
		for (InternalPacket* pck : this->packets) {
			length += pck->getLength();
		}
		return length;
	}

	void CustomPacket::decode() {
		this->packetID = this->getByte();
		this->sequenceNumber = (uint32)this->getTriad();
		this->packets = InternalPacket::fromBinary(this->buffer + this->position, this->length - 4);
	}

	void CustomPacket::encode() {
		uint32 size = this->getTotalLength();
		this->length = size;
		this->buffer = new uint8[size];

		this->putByte(this->packetID);
		this->putLTriad((int24)this->sequenceNumber);
		for (InternalPacket* pck : packets) {
			Packet temp = pck->toBinary();
			this->putByte(temp.getBuffer(), temp.getLength());
		}
	}
}
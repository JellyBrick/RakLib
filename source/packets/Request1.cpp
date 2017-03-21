#include "Request1.h"

namespace RakLib {
	Request1::Request1(std::unique_ptr<Packet> packet) : LoginPacket(std::move(packet)) {}

	void Request1::decode() {
		this->position += 17; // Skip Raknet Magic AND Packet ID
		this->version = this->getByte();
		this->mtuSize = (int16)(this->length - this->position);
	}

}
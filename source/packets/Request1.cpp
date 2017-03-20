#include "Request1.h"

namespace RakLib {
	Request1::Request1(Packet* packet) : LoginPacket(packet) {}

	void Request1::decode() {
		this->position += 17; // Skip Raknet Magic AND Packet ID
		this->version = this->getByte();
		this->mtuSize = (short)(this->length - this->position);
	}

}
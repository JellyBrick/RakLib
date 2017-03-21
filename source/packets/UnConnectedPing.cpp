#include "UnConnectedPing.h"

namespace RakLib {
	UnConnectedPing::UnConnectedPing(std::unique_ptr<Packet> pck) : LoginPacket(std::move(pck)) {}

	void UnConnectedPing::decode() {
		this->position += 1; // Skip ID
		this->pingID = this->getLong();
	}
}
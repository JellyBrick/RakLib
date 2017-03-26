#include "UnConnectedPing.h"

namespace RakLib {
	UnConnectedPing::UnConnectedPing(std::unique_ptr<RakLib::Packet>&& packet) : LoginPacket(std::move(packet)), pingID(0) {}

	void UnConnectedPing::decode() {
		this->position += 1; // Skip ID
		this->pingID = this->getULong();
	}
}
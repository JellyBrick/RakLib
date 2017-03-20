#include "UnConnectedPing.h"

namespace RakLib {
	UnConnectedPing::UnConnectedPing(Packet* pck) : LoginPacket(pck) {}

	void UnConnectedPing::decode() {
		this->position += 1; // Skip ID
		this->pingID = this->getLong();
	}
}
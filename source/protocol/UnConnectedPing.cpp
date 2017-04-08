#include "RakLib/protocol/UnConnectedPing.h"

namespace RakLib {
	UnConnectedPing::UnConnectedPing(std::unique_ptr<RakLib::Packet>&& packet) : LoginPacket(std::move(packet)), pingID(0) {}

	void UnConnectedPing::decode() {
		position += 1; // Skip Packet ID
		pingID = getULong();
	}
}
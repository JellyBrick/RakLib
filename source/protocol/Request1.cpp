#include "RakLib/protocol/Request1.h"

namespace RakLib {
	Request1::Request1(std::unique_ptr<Packet>&& packet) : LoginPacket(std::move(packet)),
		version(0x00), mtuSize(0) {}

	void Request1::decode() {
		position += 17; // Skip Raknet Magic AND Packet ID
		version = getByte();
		mtuSize = static_cast<uint16>(length - position);
	}
}
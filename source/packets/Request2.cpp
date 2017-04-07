#include "Request2.h"

namespace RakLib {
	Request2::Request2(std::unique_ptr<Packet>&& packet) 
		: LoginPacket(std::move(packet)), security(false), cookie(0), rport(0), mtuSize(0), clientID(0) {}

	void Request2::decode() {
		position += 17; // Skip Packet ID and RakNet Magic
		security = getBool();
		cookie = getInt();
		rport = static_cast<uint16>(getUShort());
		mtuSize = static_cast<uint16>(getShort());
		clientID = getLong();
	}
}
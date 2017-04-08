#include "RakLib/protocol/LoginPacket.h"

namespace RakLib {
	LoginPacket::LoginPacket(uint32 size) : Packet(size) {}

	LoginPacket::LoginPacket(std::unique_ptr<Packet>&& packet) : Packet(packet->getBuffer(), packet->getLength(), std::move(packet->ip), packet->port) {
		packet->release();
	}

	void LoginPacket::encode() {}

	void LoginPacket::decode() {}

	uint8* LoginPacket::getMagic() const {
		static uint8* RAKNET_MAGIC = new uint8[RAKNET_MAGIC_LENGTH] { 0x00, 0xff, 0xff, 0x00, 0xfe, 0xfe, 0xfe, 0xfe, 0xfd, 0xfd, 0xfd, 0xfd, 0x12, 0x34, 0x56, 0x78 };
		return RAKNET_MAGIC;
	}
}

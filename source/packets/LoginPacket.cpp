
#include "LoginPacket.h"

namespace RakLib {
	LoginPacket::LoginPacket() : Packet() {}

	LoginPacket::LoginPacket(Packet* pck) : Packet(pck) {}

	LoginPacket::LoginPacket(uint32 size) : Packet(size) {}

	void LoginPacket::encode() {}
	void LoginPacket::decode() {}

	uint8* LoginPacket::getMagic() {
		static uint8* RAKNET_MAGIC = new uint8[16] { (uint8)0x00, (uint8)0xff, (uint8)0xff, (uint8)0x00, (uint8)0xfe, (uint8)0xfe, (uint8)0xfe, (uint8)0xfe, (uint8)0xfd, (uint8)0xfd, (uint8)0xfd, (uint8)0xfd, (uint8)0x12, (uint8)0x34, (uint8)0x56, (uint8)0x78 };
		return RAKNET_MAGIC;
	}
}

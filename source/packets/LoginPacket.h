#pragma once

#include <memory>

#include "../network/Packet.h"

namespace RakLib {
	class LoginPacket : public Packet {
	public:
		static constexpr uint32 RAKNET_MAGIC_LENGTH = 16;

	public:
		LoginPacket(uint32 size);
		LoginPacket(std::unique_ptr<Packet>&& packet);

		virtual void decode();
		virtual void encode();

		uint8* getMagic() const;
	};
}

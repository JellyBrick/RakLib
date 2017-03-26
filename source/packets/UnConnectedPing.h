#pragma once

#include "LoginPacket.h"

namespace RakLib {
	class UnConnectedPing : public LoginPacket {
	public:
		uint64 pingID;

	public:
		UnConnectedPing(std::unique_ptr<Packet>&& packet);

		void decode() override;
	};
}
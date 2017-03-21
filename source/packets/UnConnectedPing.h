#pragma once

#include "LoginPacket.h"

namespace RakLib {
	class UnConnectedPing : public LoginPacket {
	public:
		int64 pingID;

	public:
		UnConnectedPing(std::unique_ptr<Packet> pck);

		void decode() override;
	};
}
#pragma once

#include "LoginPacket.h"

namespace RakLib {
	class UnConnectedPing : public LoginPacket {
	public:
		int64 pingID;

	public:
		UnConnectedPing(Packet* pck);

		void decode() override;
	};
}
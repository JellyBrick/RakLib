#pragma once

#include "LoginPacket.h"

namespace RakLib {
	class UnConnectedPong : public LoginPacket {
	public:
		int64 serverID;
		int64 pingID;
		std::string str;

		UnConnectedPong(int64 serverID, int64 pingID, const std::string& title);

		void encode() override;
	};
}
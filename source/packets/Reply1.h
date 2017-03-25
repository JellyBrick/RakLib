#pragma once

#include "LoginPacket.h"

namespace RakLib {
	class Reply1 : public LoginPacket {
	public:
		bool security;
		int64 serverID;
		uint16 mtuSize;

	public:
		Reply1(bool security, int64 serverid, uint16 mtuSize);

		void encode();
	};
}